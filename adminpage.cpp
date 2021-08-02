#include "hosetracker.h"
#include "ui_hosetracker.h"

/**
 *
 * Admin Managment page. at this point only the CSV importer exists.
 *
 */

void HoseTracker::on_CSVChooseFile_clicked(){


    QString directory = QFileDialog::getOpenFileName(this, tr("Open File"), "/documents", tr("File (*.csv)"));

    if(!directory.isEmpty()){
       // qWarning() << directory; we have an image address, so load that image into the graphics view and save the location to the label.
        ui->CSVFileLocation_Admin->setText(directory);
    }
}
void HoseTracker::on_CSVUploadFile_clicked(){ // THIS NEEDS FIXING!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //qWarning() << "Upload File Clicked";

    SQLDatabase db;

    QString table = "";
    switch(ui->CSVType_Admin->currentIndex()){
        case 0:
               // company
            table = "Companies";
            break;
        case 1:
                // home template
            table = "HoseTemplates";
            break;
        case 2:
                // fitting template
            table = "FittingTemplates";
            break;
    }

    char separator = ui->CSVSeparator_Admin->text().toStdString().c_str()[0]; // get the first one.

   // qWarning() << QString::number(ui->CSVType_Admin->currentIndex()) << separator;

    if(ui->CSVFileLocation_Admin->text() == ""){
        ui->StatusBar->setText("Error: No File Selcted");
    } else {
        ui->StatusBar->setText("Status: Uploading");
        bool upret = db.ImportFile(ui->CSVFileLocation_Admin->text(), table, separator);

        if(upret){
        ui->StatusBar->setText("Status: Ready");
        } else {
            ui->StatusBar->setText("Status: Upload Failed. Check file formatting.");
        }
    }
}
