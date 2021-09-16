// g++ egs.cpp -lcurl
// curl-config --cflags, curl-config --libs

#include <curl/curl.h>
#include <string>
#include <cstring>
#include <fstream>

struct Shive
{
	std::string url;
	size_t id;
	CURL * handle;
	bool hasReachedEnd = 0;
	std::fstream file;
};

std::string getImageLocation (const std::string &stringResult)
{
	size_t posStart, posEnd;
	posStart = stringResult.find ("id=\"cc-comic\"");
	if (posStart == std::string::npos)
		return "getImageLocation:_didnt_find_tag";
	while (stringResult [posStart] != '<' && posStart > 0)
		--posStart;
	// NOT GOOD
	// doesn't really particularly need good (?)
	posStart = stringResult.find (" src=\"", posStart) + 6;
	posEnd = stringResult.find ("\"", posStart);
	return stringResult.substr (posStart, posEnd - posStart);
}

std::string getLinkLocation (const std::string &stringResult, const char className [])
{
	size_t posStart, posEnd;
	std::string searchFor = "class=\"";
	searchFor += className;
	searchFor += "\"";
	posStart = stringResult.find (searchFor);
	if (posStart == std::string::npos)
		return "";
	while (stringResult [posStart] != '<' && posStart > 0)
		--posStart;
	posStart = stringResult.find (" href=\"", posStart) + 7; // not good, but good enough
	posEnd = stringResult.find ("\"", posStart);
	return stringResult.substr (posStart, posEnd - posStart);
}

size_t write_data (char *buffer, size_t size, size_t nmemb, Shive *userp)
{
	std::string stringResult (buffer);
	std::string imgloc = getImageLocation (stringResult);
	std::string prevcomic = getLinkLocation (stringResult, "cc-prev");
	std::string nextcomic = getLinkLocation (stringResult, "cc-next");
	printf (
		"Rqst %s\nImge %s\nCode %ld\nPrev %s\nNext %s\n\n",
		userp->url.c_str (),
		imgloc.c_str(),
		userp->id,
		prevcomic.c_str (),
		nextcomic.c_str ()
		);
	userp->file << userp->id << " "
		    << userp->url << " "
		    << imgloc << " "
		    << prevcomic << " "
		    << nextcomic << std::endl;
	if (nextcomic.length ())
	{
		userp->url = nextcomic;
		++userp->id;
		curl_easy_setopt (userp->handle, CURLOPT_URL, userp->url.c_str() );
	}
	else
	{
		userp->hasReachedEnd = true;
	}
	//curl_easy_perform (userp->handle);
	return 0;
}

// comic, sketchbook, newspaper, h

// id="cc-comic" -> image, go for brackets and src="" ?
// class="cc-prev" -> link, href, either tag will do
// if no class="cc-next" , then it's the last comic, exit

void vLoadShiveFromLine (Shive * shivep, const std::string &line)
{
	// assume [code] [url]? [other stuff, don't care]
	size_t pos1, pos2;
	pos1 = line.find (" ");
	if (pos1 == std::string::npos)
	{
		shivep->id = 1;
		shivep->url = "https://www.egscomics.com/comic/2002-01-21";
		shivep->file.seekg (0);
		return;
	}
	shivep->id = std::stoi (line.substr (0, pos1));
	++pos1;
	pos2 = line.find (" ", pos1);
	if (pos2 == std::string::npos)
		shivep->url = line.substr (pos1);
	else
		shivep->url = line.substr (pos1, pos2 - pos1);
}

void vLoadLastLine (Shive * shivep)
{
	// get the last line with anything on it
	std::string line;
	shivep->file.seekg (-1, std::ios_base::end);
	bool keepLooping = true;
	unsigned int nStep = 0;
	while (keepLooping)
	{
		char ch;
		shivep->file.get (ch);
		++nStep;
		if ((int) shivep->file.tellg () <= 1)
		{
			keepLooping = false;
			shivep->file.seekg (0);
		}
		else if (ch == '\n')
		{
			if (nStep > 1)
				keepLooping = false;
			nStep = 0;
		}
	        if (keepLooping)
			shivep->file.seekg (-2, std::ios_base::cur);
	}
	std::string lastLine;
	std::streampos pos = shivep->file.tellg ();
	getline (shivep->file, lastLine);
	shivep->file.seekg (pos, std::ios::beg);
	vLoadShiveFromLine (shivep, lastLine);
	printf ("%s\n", lastLine.c_str ());
}

int main (int argc, char ** argv)
{
	CURLcode code_result;
	Shive shive;
	if (argc == 2)
		shive.file.open (argv [1], std::ios_base::out | std::ios_base::in);
	if (shive.file.is_open ())
	{
		vLoadLastLine (&shive);
	}
	else
	{
		printf ("open file with out failed\n");
		shive.file.open (argv [1], std::ios_base::app | std::ios_base::in);
		shive.id = 1;
		shive.url = "https://www.egscomics.com/comic/2002-01-21";
	}
	if (!shive.file.is_open ())
	{
		printf ("open file with append failed\n");
		return 1;
	}
	shive.handle = curl_easy_init ();
	curl_easy_setopt (shive.handle, CURLOPT_URL, shive.url.c_str());
	curl_easy_setopt (shive.handle, CURLOPT_WRITEFUNCTION, write_data);
	curl_easy_setopt (shive.handle, CURLOPT_WRITEDATA, &shive);
	while (!shive.hasReachedEnd)
	{
		code_result = curl_easy_perform (shive.handle);
	}
	shive.file << std::endl;
	shive.file.close ();
	return 0;
}
