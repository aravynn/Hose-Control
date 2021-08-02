#ifndef DATABUTTON_H
#define DATABUTTON_H

#include <QPushButton>

#include "TrackerEnums.h"
#include "TrackerStructs.h"

/**
 *
 * Create a push button option with a single "ID" sector to pass data between pages.
 *
 */

class DataButton : public QPushButton
{
private:
    CompanyID m_id;
public:
    DataButton();
    DataButton(CompanyID id);
    CompanyID getData();
};

#endif // DATABUTTON_H
