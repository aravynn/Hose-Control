#include "hosetracker.h"
#include "ui_hosetracker.h"

/**
 *
 * Functions adding a fitting template. Will only need very few.
 *
 */


void HoseTracker::on_SaveFittingTemplate_clicked(){
    // save the fitting template
    FittingTemplate *ft = new FittingTemplate(
                ui->FittingName_AddFitting->text(),
                ui->PartNumber_AddTemplate->text(),
                ui->Notes_AddFitting->toPlainText(),
                ui->CouplingMethod_AddFitting->text());

    delete ft;

    //qWarning() << QString::number(m_IDPass);

    // return the user to the previous page.
    //loadPage(Pages::AddHoseTemplate, m_IDPass);
    loadFittingDropdowns();
    on_PageBack_clicked(); // we need to go BACK.
}
