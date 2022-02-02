#include "pdfcreator.h"

PDFCreator::PDFCreator(CompanyID id)
{
 // create the objects for the hose test, hose, company
    m_HoseTest = new HoseTest(id); // get the hose test information.

    // use the hose test information to determine the hose and company.
    m_Hose = new Hose(m_HoseTest->getHoseID()); // get the hose information.

    m_Company = new Company(m_Hose->getOwnerID()); // get the company information

    // initialize the images.
    Q_INIT_RESOURCE(pdf_images);
}

PDFCreator::~PDFCreator(){
    // clean up all created resources.
    delete m_HoseTest;
    delete m_Hose;
    delete m_Company;
    // we don't need to delete PDF, apparently it does not require a separate object.
}

bool PDFCreator::MakePDF(){
    // generate a new PDF from the existing data as called by the system.
    // apparently QT has a PDF function, but I'll roll with my own since I have it here and downloaded.
    // its all open source so no need to reinvent the wheel here.

    // determine file location on the hard drive.
    QString fileLocation = GetPDFPath(true); // true returns the blank uncreated path.

    // TESTING HERE - LOAD IMAGE DATA INTO ARRAY, OUTPUT THAT DATA.

    //qWarning() << fileLocation;

    // Create the header
    if(!makeHeader())
        return false;

    // create the footer
    if(!makeFooter())
        return false;

    // create the test data area
    std::vector<std::pair<QString, DataPass>> inserts;
    inserts.resize(8);
    TimeConvert t;
    inserts.at(0) = {"Date", t.print(m_HoseTest->getDate())};
    inserts.at(1) = {"Order #", m_HoseTest->getOrderNumber()};
    inserts.at(2) = {"Test #", m_HoseTest->getTestNumber()};
    inserts.at(3) = {"PO #", m_HoseTest->getOrderNumber()};
    inserts.at(4) = {"Target Load", (CompanyID)m_HoseTest->getTargetLoad()};
    inserts.at(5) = {"Peak Load", (CompanyID)m_HoseTest->getMaxPressure()};
    inserts.at(6) = {"Load Hold Time", (CompanyID)m_HoseTest->getHoldTime()};
    inserts.at(7) = {"Test Result", m_HoseTest->getTestResult()};
    int newTop = makeInfoArea("Test Information", inserts, 18, 0, 7); //!!!!! WE HAVE TO SET EVERYTHING UP BACKWARDS!

    // create the about hose information
    inserts.clear();
    inserts.resize(14);
    inserts.at(0) = {"Owner", m_Company->getName()};
    inserts.at(1) = {"Asset Desc", m_Hose->GetDescription()};
    inserts.at(2) = {"Asset #", m_Hose->getAssetNumber()};
    inserts.at(3) = {"Assembler", m_Hose->getCreator()};
    inserts.at(4) = {"Assembly Date", t.print(m_Hose->getMFGDate())};
    inserts.at(5) = {"Manfacturer", m_Hose->GetManufacturer()};
    inserts.at(6) = {"Model #", m_Hose->GetPartNumber()};
    inserts.at(7) = {"Inner Diameter", m_Hose->GetInnerDiameter()};
    inserts.at(8) = {"Hose Length", m_Hose->GetOverallLength()};
    inserts.at(9) = {"Coupling A", m_Hose->GetEnd1()->GetName()};
    inserts.at(10) = {"A Attachment", m_Hose->getEnd1Attachment()};
    inserts.at(11) = {"Coupling B", m_Hose->GetEnd2()->GetName()};
    inserts.at(12) = {"B Attachment",m_Hose->getEnd1Attachment()};
    inserts.at(13) = {"Hose Notes", m_Hose->GetNotes()};

    newTop = makeInfoArea("Hose Information", inserts, 18, newTop, -1);

    newTop = 190; // move to top position again.

    // output the form details
    inserts.clear();
    inserts.resize(6);
    inserts.at(0) = {"Testing By", m_HoseTest->getCreator()};
    inserts.at(1) = {"Proof Test Type", m_HoseTest->getProofTestType()};
    inserts.at(2) = {"Working Pressure", (CompanyID)m_Hose->GetWP()};
    inserts.at(3) = {"Test Pressure", (CompanyID)m_Hose->getTestPressure()};
    inserts.at(5) = {"Test Notes", m_HoseTest->getComments()};


    QDate today = m_HoseTest->getDate();
    today = today.addYears(1);

    inserts.at(4) = {"Suggested Retest",  t.print(today)};

    newTop = makeInfoArea("Additional Information", inserts, 315, newTop, -1);


    // create the visual inspection checklist.

    inserts.clear();
    inserts.resize(4);
    inserts.at(0) = {"Cover", m_HoseTest->getVisualCover()};
    inserts.at(1) = {"Tube", m_HoseTest->getVisualTube()};
    inserts.at(2) = {"Fittings", m_HoseTest->getVisualFitting()};
    inserts.at(3) = {"Crimp/Clamp", m_HoseTest->getVisualCrimp()};

    newTop = makeInfoArea("Visual Inspection", inserts, 315, newTop, -1);


    // create the test graph chart
    bool didChart = makeChart(m_HoseTest->getTestType());


    // build page 2 here.
    makeImagePage();


    // Build the PDF. This needs to be done last.
    std::string errMsg;
    if (!m_pdf.writeToFile(fileLocation.toStdString(), errMsg)){
       qWarning() << QString::fromStdString( errMsg );
       return false;
    }

    return true; // assuming that a PDF is output.
}

bool PDFCreator::makeHeader(){
    // generic function to create the header.
    // insert the logo image
    QString mepLogo = ":/pdf/img/mepbro-pdf.png";
    Image pixels;

    if(!getImageArray(mepLogo, pixels)){
        qWarning() << "Error: failed to load image";
        return false;
    }

    ImageInfo info = m_pdf.processImage(pixels);

    m_pdf.showImage(info, 18, 700, .25); // info, x, y, scale - 1.0 is 100%, file is from bottom-left


    m_pdf.setFillColor(69, 126, 192);
    m_pdf.fillRect(267, 700, 326, 75);

    // output the title, HOSE TEST CERTIFICATE, to the right side of the logo
    m_pdf.setFillColor(255,255,255);
    m_pdf.setFont(PDF::HELVETICA_BOLD, 36);
    m_pdf.showTextXY("HOSE TEST", 327, 742);
    m_pdf.showTextXY("CERTIFICATE", 310, 708);

    return true;
}

bool PDFCreator::makeChart(TestType type){
    // output the chart. Will always be same location and size.
    // width standard 279.

    m_pdf.setLineWidth(0);

    int Bottom{510}; // bottom edge
    int Left {315}; // left edge
    int Width {279};
    int Height {172};

    // in the case of a rejection we'll handle this here.
    if(type == TestType::REJECTED){

        m_pdf.setFillColor(224,224,224);
        m_pdf.setFont(PDF::HELVETICA, 54);
        m_pdf.showTextXY("REJECTED", Left, Bottom);
        m_pdf.setFillColor(231,231,231);
        m_pdf.showTextXY("REJECTED", Left, Bottom + 45);
        m_pdf.setFillColor(238,238,238);
        m_pdf.showTextXY("REJECTED", Left, Bottom + 90);
        m_pdf.setFillColor(245,245,245);
        m_pdf.showTextXY("REJECTED", Left, Bottom + 135);

        return true;
    }

    // place images of the vertical text and colors for pressure and temperature. These will be placed on
    // either edge since we cannot create vertical text with this PDF software. Maybe one day we'll try to
    // make the software ourselves.
    QString pressureVert = ":/pdf/img/pressurevert.jpg";
    QString temperatureVert = ":/pdf/img/temperaturevert.jpg";
    Image pixels;
    if(!getImageArray(pressureVert, pixels)){
        qWarning() << "Error @Chart: failed to load image";
        return false;
    }
    ImageInfo info = m_pdf.processImage(pixels);
    m_pdf.showImage(info, Left, Bottom + 65, 0.25);

    if(!getImageArray(temperatureVert, pixels)){
        qWarning() << "Error @Chart: failed to load image";
        return false;
    }
    info = m_pdf.processImage(pixels);
    m_pdf.showImage(info, Left + 272, Bottom + 60, 0.25);

    // create the bottom label
    m_pdf.setFont(PDF::HELVETICA, 6);
    m_pdf.showTextXY("Test Time (Seconds)", Left + 110, Bottom);

    // determine the total seconds for the test
    int testTime = m_HoseTest->getTestLength();

    // determine the min and max pressure
    // Determine the min and max temperature
    double minTemp = 1000.0;
    double minPres = 1000000.0;
    double maxTemp = -100.0;
    double maxPres = -100.0;
    std::vector<std::pair<double, double>> TestData; // set in a pair for each access.
    for(int i{0}; i < testTime; ++i){
        // create iterations of the data we need as well as the maximums for each.
        TestData.push_back({m_HoseTest->getPressureAtID(i), m_HoseTest->getTemperatureAtID(i)});

        if(TestData.at(i).first > maxPres){ maxPres = TestData.at(i).first; }
        if(TestData.at(i).first < minPres){ minPres = TestData.at(i).first; }

        if(TestData.at(i).second > maxTemp){ maxTemp = TestData.at(i).second; }
        if(TestData.at(i).second < minTemp){ minTemp = TestData.at(i).second; }
    }


    // update variables for simpler data entry.
    Bottom += 20;

    if(maxPres < 100){
        Left += 25;
        Width -= 25;
    } else if(maxPres < 10000){
        Left += 30;
        Width -= 30;
    } else if(maxPres < 100000){
        Left += 30;
        Width -= 30;
    } else {
        Left += 35;
        Width -= 35;
    }

    if(maxTemp < 100){
        Width -= 30;
    } else if(maxTemp < 10000){
        Width -= 35;
    } else if(maxTemp < 100000){
        Width -= 40;
    } else {
        Width -= 45;
    }
    Height -= 20;

    // place the intervals on the 3 axis, with cross lines
        // horizontal seconds axis
        // every 24 points rougly place a number.
        // divide total count by 10.
    int SE = testTime / 10;
    m_pdf.setLineColor(200,200,200);

    int widthten = std::round(Width / 10.0);
    Width = widthten * 10;
  // qWarning() << "width: " << QString::number(widthten) << "/" << QString::number(Width);

    m_pdf.showTextXY("0", Left - 2, Bottom - 10);
    m_pdf.showTextXY(std::to_string(testTime), (Left + Width) - 2, Bottom - 10);
    for(int i{0}; i < 9; ++i){
        //place seconds/10 every 24.
        m_pdf.drawLine((Left + widthten * (i + 1)), Bottom - 1, (Left + widthten * (i + 1)), Bottom + Height );
        m_pdf.showTextXY(std::to_string(SE * (i + 1)), (Left + widthten * (i + 1)) - 2, Bottom - 10);
    }

    // force more spacing for the graph data, so we don't directly touch either edge.
     maxPres *= 1.15;
     minPres *= 0.9;
     maxTemp *= 1.15;
     minTemp *= 0.9;

        // vertical pressure axis
        // vertical temperature axis
    double PA = (maxPres - minPres) / 5;
    double TA = (maxTemp - minTemp) / 5;



    m_pdf.showTextXY(DBLtoSTR(maxPres), 325, Bottom + Height - 5);
    m_pdf.showTextXY(DBLtoSTR(minPres), 325, Bottom);

    m_pdf.drawLine(Left - 1, Bottom + Height, Left + 1 + Width, Bottom + Height );

    m_pdf.showTextXY(DBLtoSTR(maxTemp), Left + Width + 5, Bottom + Height - 5);
    m_pdf.showTextXY(DBLtoSTR(minTemp), Left + Width + 5, Bottom);

    for(int i{1}; i < 5; ++i){
        m_pdf.drawLine(Left - 1, Bottom + (30 * i), Left + 1 + Width, Bottom + (30 * i) );
        m_pdf.showTextXY(DBLtoSTR(PA * i + minPres), 325, Bottom + (30 * i) - 3);
        m_pdf.showTextXY(DBLtoSTR(TA * i + minTemp), Left + Width + 5, Bottom + (30 * i) - 3);
    }

    // Draw the base lines for the graph
    m_pdf.setLineColor(0,0,0);
    m_pdf.drawLine(Left, Bottom, Left + Width, Bottom);
    m_pdf.drawLine(Left, Bottom, Left, Bottom + Height);
    m_pdf.drawLine(Left + Width, Bottom, Left + Width, Bottom + Height);

    // draw the test pressure line
    double tp = (m_HoseTest->getTargetLoad() - minPres) / (maxPres - minPres) * Height + Bottom;
    m_pdf.setLineColor(106,189,69);
    m_pdf.setLineWidth(1);

    // check here for min/max of chart.
    if(tp < Bottom){ tp = Bottom; }
    if(tp > Height + Bottom){ tp = Height + Bottom; }

    m_pdf.drawLine(Left, (int)std::round(tp), Left + Width, (int)std::round(tp));

    // draw the max pressure line
    double mp = (m_HoseTest->getMaxPressure() - minPres) / (maxPres - minPres) * Height + Bottom;
    m_pdf.setLineColor(237,32,36);
    m_pdf.setLineWidth(1);

    if(mp < Bottom){ mp = Bottom; }
    if(mp > Height + Bottom){ mp = Height + Bottom; }

    m_pdf.drawLine(Left, (int)std::round(mp), Left + Width, (int)std::round(mp));

    //qWarning() << "testp: " << QString::number(m_HoseTest->getTargetLoad()) << "maxp:" << QString::number(m_HoseTest->getMaxPressure());
    //qWarning() << "minPres: " << QString::number( minPres ) << "maxPres: " << QString::number( maxPres );

    m_pdf.setLineWidth(0);



    m_pdf.setLineColor(51,102,255);
    int a = std::round((0) / (double)testTime * Width + Left);
    int b = std::round((TestData.at(0).second - minTemp) / (maxTemp - minTemp) * Height + Bottom);
    int c = std::round((1) / (double)testTime * Width + Left);
    int d = std::round((TestData.at(0).second - minTemp) / (maxTemp - minTemp) * Height + Bottom);
    m_pdf.drawLine((int)a, (int)b, (int)c, (int)d);

    // draw the pressure line
    m_pdf.setLineColor(0,0,0);

    a = std::round((0) / (double)testTime * Width + Left);
    b = std::round((TestData.at(0).first - minPres) / (maxPres - minPres) * Height + Bottom);
    c = std::round(1 / (double)testTime * Width + Left);
    d = std::round((TestData.at(0).first - minPres) / (maxPres - minPres) * Height + Bottom);
    m_pdf.drawLine((int)a, (int)b, (int)c, (int)d);


    // draw the temperature line
    // draw the pressure line
    for(int i{1}; i < testTime; ++i){
        // create iterations of the data we need as well as the maximums for each.
        //TestData.push_back({m_HoseTest->getPressureAtID(i), m_HoseTest->getTemperatureAtID(i)});

        // draw the temperature line
        m_pdf.setLineColor(51,102,255);
        int a = std::round((i) / (double)testTime * Width + Left);
        int b = std::round((TestData.at(i - 1).second - minTemp) / (maxTemp - minTemp) * Height + Bottom);
        int c = std::round((i+1) / (double)testTime * Width + Left);
        int d = std::round((TestData.at(i).second - minTemp) / (maxTemp - minTemp) * Height + Bottom);
        m_pdf.drawLine((int)a, (int)b, (int)c, (int)d);

        // draw the pressure line
        m_pdf.setLineColor(0,0,0);

        a = std::round((i) / (double)testTime * Width + Left);
        b = std::round((TestData.at(i - 1).first - minPres) / (maxPres - minPres) * Height + Bottom);
        c = std::round((i+1) / (double)testTime * Width + Left);
        d = std::round((TestData.at(i).first - minPres) / (maxPres - minPres) * Height + Bottom);
        m_pdf.drawLine((int)a, (int)b, (int)c, (int)d);
       // qWarning() << i << testTime << Width << a << b << c << d;
    }


    return true;
}

std::string PDFCreator::DBLtoSTR(double number){
    // take the double given, and revert to a string for easy use in the final graph.
    int decimals = 100;
    // first determine what format we need: x.xx, xx.x, xxx.x, xxxx, xxxxx, xxxxxx
    if(number >= 1000.0){
        // we don't need a decimal. Return the number sounded.
        return std::to_string((int)std::round(number));
    } else if(number >= 10.0){
        // number needs only 1 decimal
        decimals = 10;
    } // otherwise, default value.

    // proceed with calculation.
    int lead = (int)std::floor(number);
    int tail = (int)std::floor(number * decimals) - lead * decimals;

   // qWarning() << "base" << QString::number(number) << "Lead" << QString::number(lead) << "tail" << QString::number(tail);
    // convert to string for output.
    return std::to_string(lead) + "." + std::to_string(tail);
}

int PDFCreator::makeInfoArea(QString Title, std::vector<std::pair<QString, DataPass>> inserts, int xPos, int yPos, int passfail){
    // Take all of the information and product the table that will be seen on the final page.
    // will return a height value for the next chart to be made if required.
    // if int passfail > -1, that is the pass/fail and must use a special if statement.
    // its a clunky solution but the best idea I've got for now.
    // width standard 279.
    // yPos is the distance from the optimal TOP position, we'll calculate from there.

    m_pdf.setFont(PDF::HELVETICA_BOLD, 10);

    int areaWidth{279};
    int sidemargin{6};
    int topmargin{6};
    int cellmargin{4};
    int longestTitle{0};
 //   int longestContent{0};

    for(int i{0}; i < (int)inserts.size(); ++i){
        // for each determine total lines etc. so we can set up positioning, we'll need to
        // first determine the total height of the area then actually process from top down.
        if(m_pdf.stringWidth(inserts.at(i).first.toStdString()) > longestTitle){
            // this is longer, replace the longest length.
            longestTitle = m_pdf.stringWidth(inserts.at(i).first.toStdString());
        }
    }

    //qWarning() << longestTitle;// << " " << longestContent;

    // using the assumed lengths of the title, determine the remaining space for the content.
    int contentSpace = areaWidth - sidemargin * 2 - longestTitle - cellmargin * 3; // 13 buffer for each side
    int totalExtraLines{0};

    for(int i{0}; i < (int)inserts.size(); ++i){

        if(inserts.at(i).second.o == DPO::STRING){ // only check for string.
            //if(m_pdf.stringWidth(inserts.at(i).second.str.toStdString()) > longestContent){
            //    longestContent = m_pdf.stringWidth(inserts.at(i).second.str.toStdString());
            //}
            int len = m_pdf.stringWidth(inserts.at(i).second.str.toStdString());

            if(len < contentSpace){
              //  ++totalLines; // only 1 line.
            } else {
                totalExtraLines += (len % contentSpace) ? len / contentSpace + 1 : len / contentSpace; // multiple lines.
                --totalExtraLines;
            }


        }

    }

    // we have enough information to determine the true "top" of our file, and we can now start placing.
    int lineHeight = 20;// height of each basic line.
    int additionalLineHeight = 11; // for multi line only.
    int mainLines = (int)inserts.size();

    int fullHeight = mainLines * lineHeight + totalExtraLines * additionalLineHeight + topmargin * 2; // 13 buffer to top and bottom.

    // now that we know the top position, we can start actually outputting the final design for this sector.
    // create the encompassing box
    m_pdf.setLineWidth(0);
    m_pdf.setLineColor(0,0,0);
    m_pdf.setFont(PDF::HELVETICA, 9);
    m_pdf.drawRect(xPos, 682 - yPos - fullHeight, areaWidth, fullHeight);
    m_pdf.setFillColor(255,255,255);
    m_pdf.fillRect(xPos + 5, 682 - yPos - 6, m_pdf.stringWidth(Title.toStdString()) + cellmargin * 3, 13);
    // place title
    m_pdf.setFillColor(0,0,0);
    m_pdf.showTextXY(Title.toStdString(), xPos + 8, 682 - yPos - 3);

    // create left-hand shaded area
    m_pdf.setFont(PDF::HELVETICA, 10);
    m_pdf.setFillColor(238,238,238);
    m_pdf.fillRect(xPos + sidemargin, 682 - yPos - fullHeight + sidemargin, longestTitle + cellmargin * 2, fullHeight - topmargin * 2);

    m_pdf.setFillColor(0,0,0);
    m_pdf.setLineColor(127,127,127);

    int maxTop = 682 - yPos - topmargin;

    // start placing content, if flag is set for passfail don't forget to check and place colored area.
    for(int i{0}; i < (int)inserts.size(); ++i){
        // for each item place as required. reduce the total extra lines as they are used.

        // Title
        m_pdf.setFont(PDF::HELVETICA_BOLD, 10);
        m_pdf.showTextXY(inserts.at(i).first.toStdString(), xPos + sidemargin + cellmargin,  maxTop - lineHeight + 6);

        // separator
        if (i != 0){
            // make a line, across the full width.
            m_pdf.drawLine(xPos + sidemargin, maxTop, xPos + areaWidth - sidemargin, maxTop);
        }

        // content
        if(i == passfail){
            // this is the pass/fail zone. set a box.
            if(inserts.at(i).second.str == "Pass" || inserts.at(i).second.str == "PASS"){
                // green box.
                m_pdf.setFillColor(112,191,74);
            } else {
                m_pdf.setFillColor(225,60,48);
                // red box.
            }
            m_pdf.fillRect(xPos + sidemargin + cellmargin * 2 + longestTitle, maxTop - lineHeight, contentSpace + cellmargin, lineHeight);
            m_pdf.setFillColor(0,0,0);
        }

        m_pdf.setFont(PDF::HELVETICA, 10);

        if(inserts.at(i).second.o == DPO::DOUBLE){
            QString n = QString::number(inserts.at(i).second.dbl);
            m_pdf.showTextXY(n.toStdString(), xPos + sidemargin + cellmargin * 3 + longestTitle,  maxTop - lineHeight + 6);
        } else if(inserts.at(i).second.o == DPO::NUMBER){
            QString n = QString::number(inserts.at(i).second.num);
            m_pdf.showTextXY(n.toStdString(), xPos + sidemargin + cellmargin * 3 + longestTitle,  maxTop - lineHeight + 6);
        } else { // string
            // this is a string, convfirm length and manage accordingly.
            if(contentSpace > m_pdf.stringWidth(inserts.at(i).second.str.toStdString())){
                m_pdf.showTextXY(inserts.at(i).second.str.toStdString(), xPos + sidemargin + cellmargin * 3 + longestTitle,  maxTop - lineHeight + 6);
            } else {
                // too wide, we'll need to break into lines.
                std::vector<std::string> wrap = m_pdf.wrapText(inserts.at(i).second.str.toStdString(), contentSpace - cellmargin, false);
                bool first{true};
                for(std::string w : wrap){
                    // output each then reduce the top by appropriate amount for the next.

                    m_pdf.showTextXY(w, xPos + sidemargin + cellmargin * 3 + longestTitle,  (first) ? maxTop - lineHeight + 6 : maxTop - additionalLineHeight + 2);

                    if(first){
                        first = false;
                        maxTop -= 5;
                    }



                    maxTop -= additionalLineHeight;
                }
                maxTop += lineHeight - 5;
            }
        }

        maxTop -= lineHeight;

    }


    return fullHeight + 18 + yPos; // new distance from top. include margin for the bottom for the next area.
}

bool PDFCreator::makeFooter(){
    // generic function to create the footer.
    // insert the 4 images

    QString contiLogo = ":/pdf/img/continental-pdf.png";
    QString starLogo = ":/pdf/img/star-pdf.png";
    QString nahadLogo = ":/pdf/img/nahad-pdf.png";
    QString ifpsLogo = ":/pdf/img/ifps-pdf.png";

    Image pixels;

    if(!getImageArray(contiLogo, pixels)){
      //  qWarning() << "Error: failed to load image";
        return false;
    }
    ImageInfo info = m_pdf.processImage(pixels);
    m_pdf.showImage(info, 18, 52, .25);

    if(!getImageArray(starLogo, pixels)){
    //    qWarning() << "Error: failed to load image";
        return false;
    }
    info = m_pdf.processImage(pixels);
    m_pdf.showImage(info, 202, 40, .25);

    if(!getImageArray(nahadLogo, pixels)){
      //  qWarning() << "Error: failed to load image";
        return false;
    }
    info = m_pdf.processImage(pixels);
    m_pdf.showImage(info, 334, 44, .25);

    if(!getImageArray(ifpsLogo, pixels)){
     //   qWarning() << "Error: failed to load image";
        return false;
    }
    info = m_pdf.processImage(pixels);
    m_pdf.showImage(info, 466, 43, .25);


    // create horizontal ruler
    m_pdf.setLineColor(0,0,0);
    m_pdf.setLineWidth(0);
    m_pdf.drawLine(18, 32, 594, 32);

    // insert phone number address and website.
    m_pdf.setFillColor(0,0,0);
    m_pdf.setFont(PDF::HELVETICA, 11);

    //qWarning() << m_pdf.stringWidth("725 Century St. WPG, MB, R3H 0M2");

    m_pdf.showTextXY("1.877.632.4118", 18, 18);
    m_pdf.showTextXY("725 Century St. WPG, MB, R3H 0M2", 215, 18);
    m_pdf.showTextXY("mepbrothers.com", 505, 18);
    return true;
}

bool PDFCreator::getImageArray(QString filepath, Image &pixels){
    // Take the image at filepath, and return to the given array &pixels as RGB structs.
    // we'll have to use the struct as prescribed in the pdf docutment.

    pixels.clear(); // in case pixels was previously used, it should be cleared out.

    QImage logo(filepath);
    int filewidth = (int)logo.width();
    int fileheight = (int)logo.height();

    //qWarning() << filepath;
    //qWarning() << filewidth << " " << fileheight;

    if(fileheight < 1 || filewidth < 1){
        return false; // process failed, we don't have an image.
    }

    pixels.resize(fileheight); // height is number of rows.

    //qWarning() << "Pixels size: " << pixels.size();

    for(int y{0}; y < fileheight; ++y){
        // go by row, then by column.
      //  pixels.at(y) = {};
      //  pixels.at(y).resize(filewidth);
        for(int x{0}; x < filewidth; ++x){
            // for each pixel, convert the value to be used in the image map.
            QColor c = logo.pixelColor(x, y);
            pixels.at(y).push_back( RGB((unsigned char)c.red(), (unsigned char)c.green(), (unsigned char)c.blue()));
        }
    }

    return true; // if file opened and everything is gotten correctly.
}

QString PDFCreator::GetPDFPath(bool blankPath){
    // get the last created PDF and return it. Since we can "regenerate" pdfs we'll need to be able to work around this.
    // this should be able to recall the PDF with a  fresh call of the PDF creator.

    HTDirectoryControl dir; // creeate the directory object. I suppose this could have been a static-only object.

    QString company = m_Company->getName(); // get the source company name.

    QString directoryRoot = dir.getPDFFolder(company); // this is the PDF folder.

    QString tn = m_HoseTest->getTestNumber(); // otherwise the next line throws error. damn reference.
    QString pdfName = dir.cleanName(tn); // we'll use the asset number, so all test results always use the same name.

    QString finalpath = directoryRoot + "/" + pdfName + ".pdf"; // initial finalpath name. if this doesn't work, we'll iterate.
    QString newpath = finalpath; // copy path, so we can get the path before the new path.
    int i{1}; // start at 1, so we get a -1 not a -0.

    while(dir.isFileExisting(newpath)){
        finalpath = newpath; // old path
        newpath = directoryRoot + "/" +  pdfName + "_" + QString::number(i) + ".pdf"; // new path
        ++i; // iterate the _#
    }
   // qWarning() << "test: " <<  finalpath << ' ' << newpath;
    if(!blankPath){
        return finalpath; // return the last created file path.
    }

   // qWarning() << finalpath << ' ' << newpath;

    return newpath; // return the uncreated filepath
}

bool PDFCreator::PDFExists(){
    // return true of the PDF file exists.

    HTDirectoryControl dir; // creeate the directory object. I suppose this could have been a static-only object.

    QString company = m_Company->getName(); // get the source company name.

    QString directoryRoot = dir.getPDFFolder(company); // this is the PDF folder.

    QString tn = m_HoseTest->getTestNumber(); // otherwise the next line throws error. damn reference.
    QString pdfName = dir.cleanName(tn); // we'll use the asset number, so all test results always use the same name.

    QString finalpath = directoryRoot + "/" + pdfName + ".pdf"; // initial finalpath name. if this doesn't work, we'll iterate.

    return dir.isFileExisting(finalpath); // return if this has been made
}

bool PDFCreator::makeImagePage(){
    //Generate an image page for the test if images are generated for the PDF. Note that this only loads if content is given.

    std::vector<QString> images = m_HoseTest->getDamageImages();

    if(images.size() < 1){ return true; } // end the check, nothing to add.
    if(images.at(0) == ""){ return true; } // no image to use.
    int maxpages = (int)images.size() / 4 + ((int)images.size() % 4 > 0 ? 1 : 0); // fuck I love c++

    for(int pages{0}; pages < maxpages; ++pages){

        m_pdf.newPage(); // make new page.

        // Create the header
        if(!makeHeader())
            return false;

        // create the footer
        if(!makeFooter())
            return false;

        // place images, 4 per page, 1 per section.
        for(int i{pages * 4}; i < (int)images.size() && i < (pages + 1) * 4; ++i){
            // 4 images max per page.
            Image pixels;// = new Image();

         //   qWarning() << images.at(i);

            if(!getImageArray(images.at(i), pixels)){
            //    qWarning() << "Error: failed to load image at image page";
                return false;
            }
            ImageInfo info = m_pdf.processImage(pixels);

            // use image info to determine width and height requirements - mWidth and mHeight

            double scale{1.00};
            double lAlign{0};
            double tAlign{0};

            if(info.mWidth > info.mHeight){
                // wider than tall
                scale = 276.0 / info.mWidth;//info.mHeight / 3.83 / info.mHeight; // 276 is our size.
                tAlign = (276 - info.mHeight * scale) / 2.0;
            } else {
                // taller than wide, or square
                scale = 276.0 / info.mHeight;// / 3.83 / info.mWidth; // 276 is our size.
               lAlign = (276 - info.mWidth * scale) / 2.0;
            }
           // qWarning() << QString::number(scale) << QString::number(lAlign) << QString::number(tAlign) << QString::number(info.mWidth) << QString::number(info.mHeight);
            switch(i - pages * 4){ // force 0 - 3
                case 0:
               //   m_pdf.setLineColor(255, 255, 0);
               //   m_pdf.drawRect(20, 406, 276, 276);
                    m_pdf.showImage(info, 20 + lAlign, 406 + tAlign, scale);
                    break;
                case 1:
               //   m_pdf.setLineColor(255, 0, 0);
               //   m_pdf.drawRect(316, 406, 276, 276);
                    m_pdf.showImage(info, 316 + lAlign, 406 + tAlign, scale);
                    break;
                case 2:
               //   m_pdf.setLineColor(0, 255, 0);
               //   m_pdf.drawRect(20, 110, 276, 276);
                    m_pdf.showImage(info, 20 + lAlign, 110 + tAlign, scale);
                    break;
                case 3:
                default:
               //   m_pdf.setLineColor(0, 0, 255);
               //   m_pdf.drawRect(316, 110, 276, 276);
                    m_pdf.showImage(info, 316 + lAlign, 110 + tAlign, scale);
                    break;
            }



           // delete pixels;
        }
    }
}
