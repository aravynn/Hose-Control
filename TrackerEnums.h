#ifndef TRACKERENUMS_H
#define TRACKERENUMS_H

/* All generic Enums for the Hose tracker application will be listed here. */

// links for the pages
enum class Pages {
    Login,
    Home,
    HoseCenter,
    CustomerCenter,
    AdminArea,
    TemplateCenter,
    AddCustomer,
    AddHoseTemplate,
    AddFittingTemplate,
    AddHose,
    TestingCenter,
    TestResults,
    Help
};

// ID flags for stored information, to "save" info between pages if needed.
enum class IDS {
    ResetAll,
    Customer,
    HoseTemplate,
    FittingTemplate,
    Hose,
    HoseTest
};

// ID Flags for the address type
enum class AddrType {
    Billing,
    Primary,
    Secondary
};

// flags for image file types
enum class ImageType {
    PNG,
    JPG,
    BMP,
    GIF,
    ERR
};

enum class IDSendType {
    Fitting,
    Hose,
    HoseTemplate,
    Customer,
    HoseTest,
    NA
};

// matches to the
enum class TestType {
    Sensor1,
    Sensor2,
    UL,
    CSA,
    REJECTED
};

enum class ColumnType {
    STRING,
    INTEGER,
    REAL,
    SKIP, // used for skipping explicit adds, in case we need it.
    CHILD,
    IMAGE
};

enum class Sync{
    INSERT,
    UPDATE,
    DELETE,
    IDCHANGE
};

#endif // TRACKERENUMS_H
