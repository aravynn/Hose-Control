Hose Control
-- 

Created by Kevin Jones for MEP Brothers Ltd.
Using QT for windows-only deployment. 

-- 

Summary: Hose Tracker, also known as Hose Control, is a large database-driven
program designed for use with an ESI-USB pressure sensor, in order to test 
hose assemblies for UL, CSA, CRN or generic quality assurance purposes. 

--

Project includes: The project works as a localized database, creating an SQLite
database for holding customer information, hose templates, constructed hoses 
and test information, with per-second data recording. The software works using 
a threaded testing function, to allow the test to be cancelled during testing. 

This software is designed to only work on windows, as it is required to use the 
ESI-USB C-based API for management of the sensor required for testing. 

-- 

Status and To-Do: This project is considered completed and is running 
version 1.1.0. This software has been running effectively for approxiately 
1 year without issue. 

To-Do: While this project is complete, it is still in need of some updates. 
This project is in need of minor updates to the user interface to clear up
minor confusion on the customers data page. 

This project would also benefit in a future version to have an update to a 
MySQL database, so that the data could be reliably transferred to a central 
database for use in an online form for customers to view hose certification 
on command. 