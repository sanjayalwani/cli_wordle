#!/usr/bin/env python3

import sqlite3
from os import path

# Constants
WORD_LIST_FILENAME = 'english_words_original_wordle.txt'
DB_NAME = '.words.db'

# Set up DB
conn = sqlite3.connect(DB_NAME) 
curs = conn.cursor()
curs.execute('''
             CREATE TABLE IF NOT EXISTS Wordles
             ([word] TEXT PRIMARY KEY, [wasUsed] INTEGER)
             ''')

curs.execute('''
             CREATE TABLE IF NOT EXISTS WordleHistory
             ([startOfUseUnix] INTEGER PRIMARY KEY, [word] TEXT,
             FOREIGN KEY(word) REFERENCES Wordles(word))
             ''') 
conn.commit()


# Load in all words to wordles
insert_statement = '''
                   INSERT INTO wordles(word, wasUsed)
                   VALUES(?,?)
                   '''

with open(WORD_LIST_FILENAME, 'r') as f:
    for word in f:
        curs.execute(insert_statement, (word.strip(), 0))
conn.commit()
print(f'Words succesfully loaded into {DB_NAME}')