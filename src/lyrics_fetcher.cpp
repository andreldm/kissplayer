#include "lyrics_fetcher.h"

#include <stdio.h>
#include <cctype>

#include <FL/Fl.H>
#include <curl/curl.h>

#include "util.h"
#include "sound.h"
#include "locale.h"

extern "C" {
    #include "tinycthread.h"
}

using namespace std;

class LyricsData
{
    public:
    Fl_Text_Buffer* lyrics_text_buffer;
    string artist;
    string title;
    int ticket;
};

int ticket = 0;

bool try_again(LyricsData* lyrics_data);
size_t writeToString(void* ptr, size_t size, size_t count, void* stream);
void upperCaseInitials(string& str);
void thread_sleep(int ms);
int thread_run(void * arg);
bool do_fetch(LyricsData* lyrics_data, bool firstTry = true);

void lyrics_fetcher_run(Fl_Text_Buffer* lyrics_text_buffer, string artist, string title)
{
    LyricsData* data = new LyricsData();
    data->artist = artist;
    data->title = title;
    data->lyrics_text_buffer = lyrics_text_buffer;
    data->ticket = ++ticket;

    // Ticket reset
    if (ticket > 10000) ticket = 0;

    thrd_t t;
    thrd_create(&t, thread_run, (void*) data);
}

void check_ticket(int thread_ticket)
{
    // If the sound is not loaded(stoped), also ignore this fetch
    if(thread_ticket != ticket || !sound_is_loaded()) {
        thrd_exit(thrd_success);
    }
}

int thread_run(void* arg)
{
    // This sleep is useful if the user is skipping songs
    thread_sleep(500);
    LyricsData* data = (LyricsData*) arg;
    do_fetch(data);
    return 0;
}

void thread_sleep (int ms)
{
    struct timespec ts;

    /* Calculate current time + ms */
    clock_gettime(TIME_UTC, &ts);
    ts.tv_nsec += ms * 1000000;
    if (ts.tv_nsec >= ms * 1000000) {
        ts.tv_sec++;
        ts.tv_nsec -= ms * 1000000;
    }

    thrd_sleep(&ts, NULL);
}

bool do_fetch(LyricsData* lyrics_data, bool firstTry)
{
    int thread_ticket = lyrics_data->ticket;
    string artist = lyrics_data->artist;
    string title = lyrics_data->title;
    Fl_Text_Buffer* lyrics_text_buffer = lyrics_data->lyrics_text_buffer;

    CURL* curl;
    string data;

    // Maybe we can upgrade this code to support
    // several sites, if one fail, try the next.
    string url = "http://lyrics.wikia.com/";

    char* url_unescaped;
    int findResult;

    // As of December 2013, these regex are valid.
    // If they change the site layout, this fetcher
    // might not work properly or not work at all!
    // DO NOT TRANSLATE THESE STRINGS
    string conditionNotFound = "This page needs content.";
    string conditionNotFound2 = "PUT LYRICS HERE";
    string conditionNotFound3 = "You have followed a link to a page that doesn't exist yet";
    string conditionRedirect = "#REDIRECT";
    string conditionRedirect2 = "#redirect";
    string conditionRedirect3 = "#Redirect";
    string conditionStart = "lyrics>";
    string conditionEnd = "&lt;/lyrics>";

    util_replace_all(artist, " ", "_");
    util_replace_all(title, " ", "_");
    util_replace_all(artist, "?", "%3F");
    util_replace_all(title, "?", "%3F");
    upperCaseInitials(artist);
    upperCaseInitials(title);

    url = url.append(artist);
    url = url.append(":");
    url = url.append(title);
    url = url.append("?action=edit");

    //cout <<"URL: " << url << endl;
    curl = curl_easy_init();
    check_ticket(thread_ticket);

    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeToString);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5);
        CURLcode res = curl_easy_perform(curl);
        check_ticket(thread_ticket);

        // Check if timed out
        if(res == CURLE_OPERATION_TIMEDOUT) {
            curl_easy_cleanup(curl);
            lyrics_text_buffer->text(_("Connection timed out!"));
            return false;
        }

        // Check if there was any problem
        if(res != CURLE_OK) {
            curl_easy_cleanup(curl);
            lyrics_text_buffer->text(_("Connection failure!"));
            //cout << "CURL Error: " << res << endl;
            return false;
        }

        // Check if it's a redirect page
        findResult = data.find(conditionRedirect);
        if(findResult == string::npos) {
            findResult = data.find(conditionRedirect2);
        }
        if(findResult == string::npos) {
            findResult = data.find(conditionRedirect3);
        }
        if(findResult != string::npos) {
            findResult = data.find("[[", findResult);
            data.erase(0, findResult + 2);
            findResult = data.find("]]");
            data.erase(findResult);

            findResult = data.find(":");
            if(findResult == string::npos) {
                lyrics_text_buffer->text(_("Error while redirecting."));
                return false;
            }

            artist = data.substr(0, findResult);
            title = data.substr(findResult + 1, data.length());
            lyrics_data->artist = artist;
            lyrics_data->title = title;

            // Don't use try_again because it cleans the strings
            return do_fetch(lyrics_data, firstTry);
        }

        // Check if the Lyrics doesn't exist
        findResult = data.find(conditionNotFound);
        if(findResult == string::npos) {
            findResult = data.find(conditionNotFound2);
        }
        if(findResult == string::npos) {
            findResult = data.find(conditionNotFound3);
        }

        if(findResult != string::npos) {
            curl_easy_cleanup(curl);

            if(firstTry) {
                bool result = try_again(lyrics_data);
                if(!result) {
                    lyrics_text_buffer->text(_("Not found :-("));
                }
            }

            return false;
        }

        // Check if the lyrics were really found
        findResult = data.find(conditionStart);
        if(findResult == -1) {
            curl_easy_cleanup(curl);

            if(firstTry) {
                bool result = try_again(lyrics_data);
                if(!result) {
                    lyrics_text_buffer->text(_("Error while fetching."));
                }
            }

            return false;
        }
        data.erase(0, findResult + conditionStart.size());

        findResult = data.find(conditionEnd);
        data.erase(findResult);

        curl_easy_cleanup(curl);
    }

    util_replace_all(artist, "_", " ");
    util_replace_all(title, "_", " ");
    artist = curl_easy_unescape(curl , artist.c_str(), 0, NULL);
    title = curl_easy_unescape(curl , title.c_str(), 0, NULL);

    string result = artist + "\n" + title + "\n" + data;
    util_replace_all(result, "\n\n\n", "\n\n");
    while(result.at(result.size()-1) == '\n') {
        result = result.substr(0, result.size()-1);
    }

    util_replace_all(result, "{{", "");
    util_replace_all(result, "}}", "");

    check_ticket(thread_ticket);
    Fl::lock ();
    if(FLAG_LYRICS) {
        lyrics_text_buffer->text(result.c_str());
    }
    Fl::unlock ();

    return true;
}

bool try_again(LyricsData* lyrics_data)
{
    check_ticket(lyrics_data->ticket);
    string _title = lyrics_data->title;
    util_erease_between(_title, "(", ")");
    util_erease_between(_title, "[", "]");
    util_replace_all(_title, "!", "");
    lyrics_data->title = _title;

    return do_fetch(lyrics_data, false);
}

size_t writeToString(void* ptr, size_t size, size_t count, void *stream)
{
    ((string*)stream)->append((char*)ptr, 0, size* count);
    return size* count;
}

void upperCaseInitials(string& str)
{
    for(int i = 0; i < str.length(); i++) {
        if(i == 0 && islower(str[i])) {
            str[i] = toupper(str[i]);
            continue;
        }
        if(str[i-1] && str[i-1] == '_' && islower(str[i])) {
            str[i] = toupper(str[i]);
            continue;
        }
    }
}
