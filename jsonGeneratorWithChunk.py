import json
import random
import string

def generate_random_string(length):
    characters = string.ascii_letters + string.digits
    return ''.join(random.choice(characters) for _ in range(length))

def get_random_movie_year():
    return random.randint(2001, 2011)

def get_random_book_year():
    return random.randint(1997, 2007)

def get_random_age():
    return random.randint(11, 114)

def get_random_day():
    return random.randint(1, 31)

def get_random_month():
    return random.randint(1, 12)

def get_random_duration():
    return str(random.randint(1, 3)) + 'h' + str(random.randint(1, 59)) + 'min'

def get_random_price():
    return round(random.uniform(15.00, 55.00), 2)

def get_random_pages():
    return random.randint(200, 850)

def main():
    objects = int(input("Objects: "))
    chunk_size = int(input("Chunk size: "))

    with open("data.json", "w") as file:
        file.write('{\n')

        for i in range(objects):
            file.write(f'    "hogwarts_will_always_be_there": ')
            file.write('{\n')
            file.write('        "media": [\n')
            file.write('            {\n')
            file.write('                "type": "cinematography",\n')
            file.write('                "movie": {\n')
            file.write(f'                    "title": "{generate_random_string(21)}",\n')
            file.write(f'                    "director": "{generate_random_string(21)}",\n')
            file.write(f'                    "year": {get_random_movie_year()},\n')
            file.write('                    "premiere_date": [\n')
            file.write(f'                        {get_random_month()},\n')
            file.write(f'                        {get_random_day()},\n')
            file.write(f'                        {get_random_movie_year()}\n')
            file.write('                    ],\n')
            file.write(f'                    "duration": "{get_random_duration()}",\n')
            file.write(f'                    "price": {get_random_price()},\n')
            file.write(f'                    "hey": "hey",\n')
            file.write('                    "genres": ')
            if (i + 1) % 2 == 0:
                file.write('["adventure", "mystery"]\n')
            else:
                file.write('["fantasy", "drama"]\n')
            file.write('                }\n')
            file.write('            },\n')
            file.write('            {\n')
            file.write('                "type": "text",\n')
            file.write('                "book": {\n')
            file.write(f'                    "title": "{generate_random_string(21)}",\n')
            file.write(f'                    "author": "{generate_random_string(21)}",\n')
            file.write(f'                    "year": {get_random_book_year()},\n')
            file.write('                    "premiere_date": [\n')
            file.write(f'                        {get_random_month()},\n')
            file.write(f'                        {get_random_day()},\n')
            file.write(f'                        {get_random_book_year()}\n')
            file.write('                    ],\n')
            file.write(f'                    "pages": {get_random_pages()},\n')
            file.write(f'                    "price": {get_random_price()},\n')
            file.write(f'                    "hey": "hey",\n')
            file.write('                    "genres": ')
            if (i + 1) % 2 == 0:
                file.write('["adventure", "mystery"]\n')
            else:
                file.write('["fantasy", "drama"]\n')
            file.write('                }\n')
            file.write('            }\n')

            file.write('        ],\n')
            file.write('        "characters": {\n')
            for _ in range(6):
                file.write(f'            "{generate_random_string(21)}": {get_random_age()}')
                if _ < 5:
                    file.write(',\n')
                else:
                    file.write('\n')
            file.write('        }\n')

            file.write('    }')

            if (i + 1) % chunk_size == 0 or i == objects - 1:
                file.write('\n')

            if i != objects - 1:
                file.write(',\n')

        file.write('}\n')

if __name__ == "__main__":
    main()
