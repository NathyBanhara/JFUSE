/*#include <iostream>
#include <iomanip>
#include <fstream>
#include "include/rapidjson/document.h"
#include "include/rapidjson/writer.h"
#include "include/rapidjson/stringbuffer.h"
#include "include/rapidjson/prettywriter.h"
#include "include/rapidjson/stringbuffer.h"
#include <vector>
#include <random>
#include <set>

using namespace std;
using namespace rapidjson;

string getRandomTitle()
{
    set<string> titles;
    titles.insert("Harry Potter and the Sorcerer's Stone");
    titles.insert("Harry Potter and the Chamber of Secrets");
    titles.insert("Harry Potter and the Prisoner of Askaban");
    titles.insert("Harry Potter and the Goblet of Fire");
    titles.insert("Harry Potter and the Order of the Phoenix");
    titles.insert("Harry Potter and the Half-blood Prince");
    titles.insert("Harry Potter and the Deathly Hallows");

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, titles.size() - 1);

    int randomIndex = dis(gen);

    auto it = titles.begin();
    advance(it, randomIndex);
    string randomTitle = *it;

    return randomTitle;
}

string generateRandomString(int length) {
    string characters = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    mt19937 gen(std::random_device{}());
    uniform_int_distribution<> dis(0, characters.size() - 1);

    std::string randomString;
    randomString.reserve(length);

    for (int i = 0; i < length; ++i) {
        randomString += characters[dis(gen)];
    }

    return randomString;
}

int getRandomMovieYear()
{
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(2001, 2011);
    int random_number = dis(gen);

    return random_number;
}

int getRandomBookYear()
{
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(1997, 2007);
    int random_number = dis(gen);

    return random_number;
}

int getRandomAge()
{
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(11, 114);
    int random_number = dis(gen);

    return random_number;
}

int getRandomDay()
{
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(1, 31);
    int random_number = dis(gen);

    return random_number;
}

int getRandomMonth()
{
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(1, 12);
    int random_number = dis(gen);

    return random_number;
}

string getRandomDuration()
{
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(1, 3);
    int random_number = dis(gen);

    return to_string(random_number);
}

double getRandomPrice()
{
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<double> dis(15.00, 55.00);
    double random_double = dis(gen);

    return random_double;
}

int getRandomPages()
{
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(200, 850);
    int random_number = dis(gen);

    return random_number;
}

int main()
{
    /*{
        "media": [{
            "type": "cinematography",
            "movie": {
                "title": "Harry Potter and the Goblet of Fire",
                "director": "Mike Neyell",
                "year": 2005,
                "premiere_date": [11, 25, 2005],
                "duration": "2h37min",
                "price": "15,00",
                "genres": ["fantasy", "adventures"]
            }
        },
        {
            "type": "text",
            "book": {
                "title": "Harry Potter and the Goblet of Fire",
                "author": "J.K. Rowling",
                "year": 2000,
                "premiere_date": [6, 8, 2008],
                "pages": 480,
                "price": "25,00",
                "genres": ["fantasy", "adventure"]
            }
        }],
        "characters": {
            "Harry Potter": 14,
            "Hermione Granger": 14,
            "Ronald Weasley": 14,
            "Sirius Black": 46,
            "Albus Percival Wulfric Brian Dumbledore": 114
        }
    }*/

    // object: hogwarts_will_always_be_there

    /*int objects = 100000;

    Document json;
    json.SetObject();

    std::ofstream ofs("data.json");
    if (!ofs.is_open()) {
        std::cerr << "Failed to open file for writing." << std::endl;
        return 1;
    }

    for (int i = 0; i < objects; i++)
    {
        Value mainObj(kObjectType);
        Value media(kArrayType);

        string title = generateRandomString(21);

        //obj1
        int movieYear = getRandomMovieYear();

        Value obj1(kObjectType);
        obj1.AddMember("type", "cinematography", json.GetAllocator());
        Value movie(kObjectType);
        movie.AddMember("title", Value().SetString(title.c_str(), json.GetAllocator()), json.GetAllocator());
        movie.AddMember(
            "director",
            Value().SetString(generateRandomString(21).c_str(), json.GetAllocator()),
            json.GetAllocator());
        movie.AddMember("year", movieYear, json.GetAllocator());

        Value moviePremiereDate(kArrayType);
        moviePremiereDate.PushBack(getRandomMonth(), json.GetAllocator());
        moviePremiereDate.PushBack(getRandomDay(), json.GetAllocator());
        moviePremiereDate.PushBack(movieYear, json.GetAllocator());

        movie.AddMember("premiere_date", moviePremiereDate, json.GetAllocator());
        movie.AddMember("duration", Value().SetString(getRandomDuration().c_str(), json.GetAllocator()), json.GetAllocator());
        movie.AddMember("price", getRandomPrice(), json.GetAllocator());

        Value movieGenres(kArrayType);
        if (i%2)
        {
            movieGenres.PushBack("fantasy", json.GetAllocator());
            movieGenres.PushBack("drama", json.GetAllocator());
        }
        else
        {
            movieGenres.PushBack("adventure", json.GetAllocator());
            movieGenres.PushBack("mystery", json.GetAllocator());
        }
        movie.AddMember("genres", movieGenres, json.GetAllocator());

        obj1.AddMember("movie", movie, json.GetAllocator());
        //obj1.AddMember("type2", getRandomDay(), json.GetAllocator());
        //obj1.AddMember("type3", getRandomDay(), json.GetAllocator());
        //obj1.AddMember("type4", getRandomDay(), json.GetAllocator());
        //obj1.AddMember("type5", getRandomDay(), json.GetAllocator());
        //obj1.AddMember("type6", getRandomDay(), json.GetAllocator());
        //obj1.AddMember("type7", getRandomDay(), json.GetAllocator());
        //obj1.AddMember("type8", getRandomDay(), json.GetAllocator());
        //obj1.AddMember("type9", getRandomDay(), json.GetAllocator());
        //obj1.AddMember("type10", getRandomDay(), json.GetAllocator());
        //obj1.AddMember("type11", getRandomDay(), json.GetAllocator());
        //obj1.AddMember("type12", getRandomDay(), json.GetAllocator());

        media.PushBack(obj1, json.GetAllocator());

        //obj2
        int bookYear = getRandomBookYear();

        Value obj2(kObjectType);
        obj2.AddMember("type", "text", json.GetAllocator());
        Value book(kObjectType);
        book.AddMember("title", Value().SetString(title.c_str(), json.GetAllocator()), json.GetAllocator());
        book.AddMember(
            "author",
            Value().SetString(generateRandomString(21).c_str(), json.GetAllocator()),
            json.GetAllocator());
        book.AddMember("year", bookYear, json.GetAllocator());

        Value bookPremiereDate(kArrayType);
        bookPremiereDate.PushBack(getRandomMonth(), json.GetAllocator());
        bookPremiereDate.PushBack(getRandomDay(), json.GetAllocator());
        bookPremiereDate.PushBack(bookYear, json.GetAllocator());

        book.AddMember("premiere_date", bookPremiereDate, json.GetAllocator());
        book.AddMember("pages", getRandomPages(), json.GetAllocator());
        book.AddMember("price", getRandomPrice(), json.GetAllocator());

        Value bookGenres(kArrayType);
        if (i%2)
        {
            bookGenres.PushBack("fantasy", json.GetAllocator());
            bookGenres.PushBack("drama", json.GetAllocator());
        }
        else
        {
            bookGenres.PushBack("adventure", json.GetAllocator());
            bookGenres.PushBack("mystery", json.GetAllocator());
        }
        book.AddMember("genres", bookGenres, json.GetAllocator());

        obj2.AddMember("book", book, json.GetAllocator());
        //obj2.AddMember("type2", getRandomDay(), json.GetAllocator());
        //obj2.AddMember("type3", getRandomDay(), json.GetAllocator());
        //obj2.AddMember("type4", getRandomDay(), json.GetAllocator());
        //obj2.AddMember("type5", getRandomDay(), json.GetAllocator());
        //obj2.AddMember("type6", getRandomDay(), json.GetAllocator());
        //obj2.AddMember("type7", getRandomDay(), json.GetAllocator());
        //obj2.AddMember("type8", getRandomDay(), json.GetAllocator());
        //obj2.AddMember("type9", getRandomDay(), json.GetAllocator());
        //obj2.AddMember("type10", getRandomDay(), json.GetAllocator());
        //obj2.AddMember("type11", getRandomDay(), json.GetAllocator());
        //obj2.AddMember("type12", getRandomDay(), json.GetAllocator());

        media.PushBack(obj2, json.GetAllocator());

        mainObj.AddMember("media", media, json.GetAllocator());
        Value characters(kObjectType);
        if (i%2)
        {
            characters.AddMember(Value().SetString(generateRandomString(21).c_str(), json.GetAllocator()), getRandomAge(), json.GetAllocator());
            characters.AddMember(Value().SetString(generateRandomString(21).c_str(), json.GetAllocator()), getRandomAge(), json.GetAllocator());
            characters.AddMember(Value().SetString(generateRandomString(21).c_str(), json.GetAllocator()), getRandomAge(), json.GetAllocator());
            characters.AddMember(Value().SetString(generateRandomString(21).c_str(), json.GetAllocator()), getRandomAge(), json.GetAllocator());
            characters.AddMember(Value().SetString(generateRandomString(21).c_str(), json.GetAllocator()), getRandomAge(), json.GetAllocator());
            characters.AddMember(Value().SetString(generateRandomString(21).c_str(), json.GetAllocator()), getRandomAge(), json.GetAllocator());
        }
        else
        {
            characters.AddMember(Value().SetString(generateRandomString(21).c_str(), json.GetAllocator()), getRandomAge(), json.GetAllocator());
            characters.AddMember(Value().SetString(generateRandomString(21).c_str(), json.GetAllocator()), getRandomAge(), json.GetAllocator());
            characters.AddMember(Value().SetString(generateRandomString(21).c_str(), json.GetAllocator()), getRandomAge(), json.GetAllocator());
            characters.AddMember(Value().SetString(generateRandomString(21).c_str(), json.GetAllocator()), getRandomAge(), json.GetAllocator());
            characters.AddMember(Value().SetString(generateRandomString(21).c_str(), json.GetAllocator()), getRandomAge(), json.GetAllocator());
            characters.AddMember(Value().SetString(generateRandomString(21).c_str(), json.GetAllocator()), getRandomAge(), json.GetAllocator());
            characters.AddMember(Value().SetString(generateRandomString(21).c_str(), json.GetAllocator()), getRandomAge(), json.GetAllocator());
            characters.AddMember(Value().SetString(generateRandomString(21).c_str(), json.GetAllocator()), getRandomAge(), json.GetAllocator());
            characters.AddMember(Value().SetString(generateRandomString(21).c_str(), json.GetAllocator()), getRandomAge(), json.GetAllocator());
            characters.AddMember(Value().SetString(generateRandomString(21).c_str(), json.GetAllocator()), getRandomAge(), json.GetAllocator());
            characters.AddMember(Value().SetString(generateRandomString(21).c_str(), json.GetAllocator()), getRandomAge(), json.GetAllocator());
            characters.AddMember(Value().SetString(generateRandomString(21).c_str(), json.GetAllocator()), getRandomAge(), json.GetAllocator());
            characters.AddMember(Value().SetString(generateRandomString(21).c_str(), json.GetAllocator()), getRandomAge(), json.GetAllocator());
            characters.AddMember(Value().SetString(generateRandomString(21).c_str(), json.GetAllocator()), getRandomAge(), json.GetAllocator());
            characters.AddMember(Value().SetString(generateRandomString(21).c_str(), json.GetAllocator()), getRandomAge(), json.GetAllocator());
            characters.AddMember(Value().SetString(generateRandomString(21).c_str(), json.GetAllocator()), getRandomAge(), json.GetAllocator());
            characters.AddMember(Value().SetString(generateRandomString(21).c_str(), json.GetAllocator()), getRandomAge(), json.GetAllocator());
            characters.AddMember(Value().SetString(generateRandomString(21).c_str(), json.GetAllocator()), getRandomAge(), json.GetAllocator());
        }

        mainObj.AddMember("characters", characters, json.GetAllocator());

        json.AddMember("hogwarts_will_always_be_there", mainObj, json.GetAllocator());

        if ((i + 1) % 100 == 0 || i == objects - 1) {
            StringBuffer buffer;
            PrettyWriter<StringBuffer> writer(buffer);
            writer.SetFormatOptions(PrettyFormatOptions::kFormatDefault);
            json.Accept(writer);
            ofs << buffer.GetString();
            if (i != objects - 1) {
                ofs << ',';
            }
            ofs << std::endl;

            json.SetObject();
        }
    }

    ofs.close();

    return 0;
}*/