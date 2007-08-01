#ifndef MESSAGEBOX_H
#define MESSAGEBOX_H

#define GENERIC_TRY_CATCH( BLOCK ) \
try { BLOCK } catch(Error E) {	\
	message_box("Error", E.what(), "OK");	\
} catch(exception e) { \
	message_box("Error", e.what(), "OK");   \
}

void message_box(const char *title, const char *what, const char *button);
#endif
