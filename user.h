#ifndef USER_H
#define USER_H

#include <QString>

/**
 * User Class
 *
 * This class manages the user login for the LOCAL system only.
 * This will also produce/decode the strings for the user's password
 * Once a user is entered, it will remained stored for future access
 * and transferring to the website link. See notes on data transfer.
 *
 */

class User
{
private:
public:
    User(QString username, QString password);
    ~User();
};

#endif // USER_H
