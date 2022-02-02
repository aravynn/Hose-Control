#include "hosetracker.h"
#include "ui_hosetracker.h"

/**
 *
 * Admin Managment page. at this point only the CSV importer exists.
 *
 */

void HoseTracker::on_CSVChooseFile_clicked(){
    // open a dialog to choose a CSV file for upload of mass information.

    QString directory = QFileDialog::getOpenFileName(this, tr("Open File"), "/documents", tr("File (*.csv)"));

    if(!directory.isEmpty()){
       // qWarning() << directory; we have an image address, so load that image into the graphics view and save the location to the label.
        ui->CSVFileLocation_Admin->setText(directory);
    }
}
void HoseTracker::on_CSVUploadFile_clicked(){
    // on click upload the file into the databse, based on selected options.

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
