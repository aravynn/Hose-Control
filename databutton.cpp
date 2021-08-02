#include "databutton.h"

DataButton::DataButton()
{
    // default instantiation.
}

DataButton::DataButton(CompanyID id) : m_id{id}
{
    // instantiate with given ID for buttons.
}

CompanyID DataButton::getData(){
    return m_id;
}
