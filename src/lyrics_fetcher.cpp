#include "lyrics_fetcher.h"

#include <stdio.h>
#include <cctype>
#include <regex>

#include <FL/Fl.H>
#include <curl/curl.h>

#include "configuration.h"
#include "util.h"
#include "locale.h"

extern "C" {
    #include "tinycthread.h"
}

using namespace std;

class LyricsData {
public:
    Fl_Text_Buffer* text_buffer;
    Sound* sound;
    string artist;
    string title;
    string proxy;
    int ticket;
    bool shouldFetchLyrics;
};

int ticket = 0;

bool try_again(LyricsData* lyrics_data);
void thread_sleep(int ms);
int thread_run(void * arg);
bool do_fetch(LyricsData* lyrics_data, bool firstTry = true);

LyricsFetcher::LyricsFetcher(Context* context, Fl_Text_Buffer* text_buffer)
{
    this->text_buffer = text_buffer;
}

void LyricsFetcher::fetch(Music* music) {
    string proxy = context->dao->open_get_key("proxy");

    LyricsData* data = new LyricsData();
    data->text_buffer = text_buffer;
    data->artist = music->artist;
    data->title = music->title;
    data->sound = context->sound;
    data->proxy = proxy;
    data->ticket = ++ticket;
    data->shouldFetchLyrics = context->configuration->shouldFetchLyrics();

    // Ticket reset
    if (ticket > 10000) ticket = 0;

    thrd_t t;
    thrd_create(&t, thread_run, (void*) data);
}

void check_ticket(int thread_ticket, Sound* sound) {
    // If the sound is not loaded(stopped), also ignore this fetch
    if (thread_ticket != ticket || !sound->isLoaded()) {
        thrd_exit(thrd_success);
    }
}

int thread_run(void* arg) {
    // This sleep is useful if the user is skipping songs
    thread_sleep(500);
    LyricsData* data = (LyricsData*) arg;
    do_fetch(data);
    return 0;
}

void thread_sleep (int ms) {
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

bool do_fetch(LyricsData* lyrics_data, bool firstTry) {
    int thread_ticket = lyrics_data->ticket;
    string artist = lyrics_data->artist;
    string title = lyrics_data->title;
    string proxy = lyrics_data->proxy;
    Fl_Text_Buffer* text_buffer = lyrics_data->text_buffer;

    CURL* curl;
    string data;
    uint findResult;

    // Maybe we can upgrade this code to support
    // several sites, if one fail, try the next.
    string url = "http://lyrics.wikia.com/";

    // As of November 2014, these regex are valid.
    // If they change the site layout, this fetcher
    // might not work properly or not work at all!
    // DO NOT MAKE THESE STRINGS TRANSLATABLE
    smatch m;
    regex regexNotFound ("This page needs content\\.");
    regex regexNotFound2 ("PUT LYRICS HERE");
    regex regexNotFound3 ("You have followed a link to a page that doesn't exist yet");
    regex regexRedirect ("#redirect", regex_constants::icase);
    regex regexFound ("lyrics>(.*)&lt;/lyrics>", regex_constants::extended);
    string conditionStart = "lyrics>";
    string conditionEnd = "&lt;/lyrics>";

    util_replace_all(artist, " ", "_");
    util_replace_all(title, " ", "_");
    util_replace_all(artist, "?", "%3F");
    util_replace_all(title, "?", "%3F");
    util_uppercase_initials(artist);
    util_uppercase_initials(title);

    url = url.append(artist);
    url = url.append(":");
    url = url.append(title);
    url = url.append("?action=edit");

    // cout <<"URL: " << url << endl;
    curl = curl_easy_init();
    check_ticket(thread_ticket, lyrics_data->sound);

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, util_write_string);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5);

        if (!proxy.empty()) {
            curl_easy_setopt(curl, CURLOPT_PROXY, proxy.c_str());
        }
        CURLcode res = curl_easy_perform(curl);
        check_ticket(thread_ticket, lyrics_data->sound);

        // Check if timed out
        if (res == CURLE_OPERATION_TIMEDOUT) {
            curl_easy_cleanup(curl);
            text_buffer->text(_("Connection timed out!"));
            return false;
        }

        // Check if there was any problem
        if (res != CURLE_OK) {
            curl_easy_cleanup(curl);
            text_buffer->text(_("Connection failure!"));
            //cout << "CURL Error: " << res << endl;
            return false;
        }

        // Check if it's a redirect page
        if (regex_match(data, regexRedirect)) {
            findResult = data.find("[[", findResult);
            data.erase(0, findResult + 2);
            findResult = data.find("]]");
            data.erase(findResult);

            findResult = data.find(":");
            if (findResult == string::npos) {
                text_buffer->text(_("Error while redirecting."));
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
        if (regex_search(data, regexNotFound) ||
			regex_search(data, regexNotFound2) ||
			regex_search(data, regexNotFound3)) {
            curl_easy_cleanup(curl);

            if (firstTry) {
                bool result = try_again(lyrics_data);
                if (!result) {
                    text_buffer->text(_("Not found :-("));
                }
            }

            return false;
        }

        // Check if the lyrics were really found
        if (regex_search(data, m, regexFound) && m.size() > 1) {
			data = m.str(1);
		} else {
            curl_easy_cleanup(curl);

            if (firstTry) {
                bool result = try_again(lyrics_data);
                if (!result) {
                    text_buffer->text(_("Error while fetching."));
                }
            }

            return false;
        }

        curl_easy_cleanup(curl);
    }

    util_replace_all(artist, "_", " ");
    util_replace_all(title, "_", " ");
    artist = curl_easy_unescape(curl , artist.c_str(), 0, NULL);
    title = curl_easy_unescape(curl , title.c_str(), 0, NULL);

    string result = artist + "\n" + title + "\n" + data;
    util_replace_all(result, "\n\n\n", "\n\n");
    while (result.at(result.size()-1) == '\n') {
        result = result.substr(0, result.size()-1);
    }

    util_replace_all(result, "{{", "");
    util_replace_all(result, "}}", "");

    check_ticket(thread_ticket, lyrics_data->sound);
    Fl::lock();
    if (lyrics_data->shouldFetchLyrics) {
        text_buffer->text(result.c_str());
    }
    Fl::unlock();

    return true;
}

bool try_again(LyricsData* lyrics_data) {
    check_ticket(lyrics_data->ticket, lyrics_data->sound);
    string _title = lyrics_data->title;
    util_erase_between(_title, "(", ")");
    util_erase_between(_title, "[", "]");
    util_replace_all(_title, "!", "");
    lyrics_data->title = _title;

    return do_fetch(lyrics_data, false);
}
