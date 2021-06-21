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
		return "getImageLocation: tag not found";
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
	userp->file << "Rqst " << userp->url << std::endl
		    << "Imge " << imgloc << std::endl
		    << "Code " << userp->id << std::endl
		    << "Prev " << prevcomic << std::endl
		    << "Next " << nextcomic << std::endl
		    << std::endl;
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

int main (int argc, char ** argv)
{
	//CURLcode handle;
	CURLcode code_result;
	//handle = curl_global_init (CURL_GLOBAL_SSL);
	//CURL * handle;
	Shive shive;
	if (argc == 1)
	{
		shive.url = "https://www.egscomics.com/comic/2002-01-21";
		shive.id = 4; // as per ookii
	}
	else if (argc == 3)
	{
		shive.id = std::stoi (argv [1]);
		shive.url = argv [2];
	}
	shive.file.open ("egsout.txt", std::ios_base::app | std::ios_base::in);
	if (!shive.file.is_open ())
	{
		return 1;
	}
	//printf ("%d %s\n", argc, shive.url.c_str () );
	shive.handle = curl_easy_init ();
	//char url [100] = "https://www.egscomics.com/comic/balance-027";
	curl_easy_setopt (shive.handle, CURLOPT_URL, shive.url.c_str());
	curl_easy_setopt (shive.handle, CURLOPT_WRITEFUNCTION, write_data);
	curl_easy_setopt (shive.handle, CURLOPT_WRITEDATA, &shive);
	while (!shive.hasReachedEnd)
	{
		code_result = curl_easy_perform (shive.handle);
	}
	return 0;
}
