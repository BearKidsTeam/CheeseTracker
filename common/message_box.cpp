#include <qmessagebox.h>

// Needed because sound_driver_manager.cpp won't compile if
// <qmessagebox.h> is included.

void message_box(const char *title, const char *what, const char *button)
{
	QMessageBox::critical(NULL, title, what, button);
}

