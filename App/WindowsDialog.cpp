#include "WindowsDialog.h"
std::string GetFileName(const std::string & prompt) 
{ 
    const int BUFSIZE = 1024;
    char buffer[BUFSIZE] = {0};
    OPENFILENAME ofns = {0};
	ofns.hwndOwner = FileDialog::handle;
    ofns.lStructSize = sizeof( ofns );
    ofns.lpstrFile = buffer;
    ofns.nMaxFile = BUFSIZE;
    ofns.lpstrTitle = prompt.c_str();
	if (GetOpenFileName(&ofns))
		return buffer;
	else
		return "ERROR";
}
HWND FileDialog::handle;