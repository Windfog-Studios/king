#include "p2Log.h"
#include "j1Console.h"
#include "j1App.h"
#include "p2SString.h"

void log(const char file[], int line, const char* format, ...)
{
	static char tmp_string[4096];
	static char tmp_string2[4096];
	static va_list  ap;

	// Construct the string from variable arguments
	va_start(ap, format);
	vsprintf_s(tmp_string, 4096, format, ap);
	va_end(ap);
	sprintf_s(tmp_string2, 4096, "\n%s(%d) : %s", file, line, tmp_string);
	OutputDebugString(tmp_string2);

	if (App != nullptr)
	{
		if (App->console->log_record.start != nullptr) {
			p2SString converted_text(tmp_string2);
			converted_text;
			App->console->AddLogText(converted_text);
		}
	}

}