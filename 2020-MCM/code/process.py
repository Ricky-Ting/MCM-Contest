import pandas as pd 
import nltk
from nltk.corpus import stopwords
from nltk.stem import PorterStemmer
from nltk.tokenize import sent_tokenize, word_tokenize

import numpy as np
from wordcloud import WordCloud,STOPWORDS,ImageColorGenerator
from PIL import Image
from matplotlib import pyplot as plt
import random

from textblob.classifiers import NaiveBayesClassifier

from ast import literal_eval as createTuple

from textblob import TextBlob


file_name = ['hair_dryer', 'pacifier', 'microwave']
must_contain = ['dryer', 'pacifier', 'microwave']

cur_file = 0
save_train_enable = False
train_enable = False
test_enable = False
word_cloud_enable = True

# get text


def wc_english(text, filename):
    wc = WordCloud(scale=2,max_font_size = 100)
    # 生成词云
    wc.generate_from_text(text)

    wc.to_file(filename)

    #存储图像
    #wc.to_file('1900pro1.png')
    # 显示图像
    plt.imshow(wc,interpolation='bilinear')
    plt.axis('off')
    plt.tight_layout()
    plt.show()


def parse_year(date):
    day_s, mon_s, year_s = (date).split('/')
    return int(year_s)


def get_csv(filename, must_contain):
    data = pd.read_csv(filename + '.tsv', sep='\t', header=0)
    data = data[data['product_title'].str.contains(must_contain)]
    if save_train_enable:
        save_train(data, filename)
    return data


def save_train(data, filename):
    cnt1 = 0
    cnt5 = 0
    for index, row in data.iterrows():
        if parse_year(row.review_date) < 2014:
            if row.star_rating == 1:
                cnt1 = cnt1 + 1
            if row.star_rating == 5:
                cnt5 = cnt5 + 1
    print(cnt1)
    print(cnt5)

    output = ''
    #cnt = min(cnt1, cnt5)
    for index, row in data.iterrows():
        if parse_year(row.review_date) < 2014:
            if row.star_rating==5:
                output += '{}|pos\n'.format(row.review_body)
            elif row.star_rating==1:
                output += '{}|neg\n'.format(row.review_body)
    f = open(filename+'_train.txt', 'w')
    f.write(output)
    f.close()

def get_text(data):
    text = ''
    for line in data['review_body']:
        text = text + str(line)
    return text


def get_tokens(text):
    stop_words = set(stopwords.words('english'))
    my_stopwords = ['hair', 'dryer', 'baby', 'bought', 'br', 'microwave', 'one']
    for sw in my_stopwords:
        stop_words.add(sw)

    #word_tokens = word_tokenize(text)
    word_tokens = nltk.regexp_tokenize(text, r'[a-zA-Z]+')

    filtered_sentence = [w for w in word_tokens if not w in stop_words]

    tags = nltk.pos_tag(filtered_sentence)

    addj = []
    for tag in tags:
        if tag[1] == 'JJ':
            addj.append(tag[0])

    return addj
    

def get_train_set(filename):
    train = []
    f = open(filename+'_train.txt', 'r')
    for line in f:
        line = line.replace("\n", "")
        a, b = line.split('|')
        train.append((a, b))
    f.close()
    return train


data = get_csv(file_name[cur_file], must_contain[cur_file])
text = get_text(data)

# train model
if train_enable:
    train = get_train_set(file_name[cur_file])
    cl = NaiveBayesClassifier(train)
    cl.show_informative_features(1000)
    if test_enable:
        data['sentiment'] = 0.0
        for index, row in data.iterrows():
            #print(row)
            #print(cl.prob_classify(row.review_body).prob("pos"))
            val1 = TextBlob(str(row.review_body)).sentiment.polarity
            val2 = (cl.prob_classify(str(row.review_body)).prob("pos") - 0.5) // 0.5
            data.at[index, 'sentiment'] = 0.3*val1 + 0.7*val2
            #data.at[index, 'sentiment'] = TextBlob(str(row.review_body)).sentiment.polarity
            #data.at[index, 'sentiment'] = round(cl.prob_classify(str(row.review_body)).prob("pos"), 5)
        data.to_csv(file_name[cur_file]+'_sentiment.csv')

if word_cloud_enable:
    f = open(file_name[cur_file]+'_feature.txt', 'r')
    pos_words = {}
    neg_words = {}
    for line in f:
        line = line.replace('\n', "")
        line = line.replace('(', " ")
        line = line.replace(')', " ")
        strs = line.split(' ')
        words = [s for s in strs if s != '']
        if(words[4] == 'neg'):
            neg_words[words[1]] = int(float(words[8])*10)
        else:
            pos_words[words[1]] = int(float(words[8])*10)
    f.close()

    output1 = ''
    output1 = output1 + '[ '
    neg_word_cloud = []
    for key, value in neg_words.items():
        output1 = output1 + key + ", "
        for i in range(value):
            neg_word_cloud.append(key)
    random.shuffle(neg_word_cloud)
    wc_english((',').join(neg_word_cloud), file_name[cur_file]+'_neg_wordcloud.png')
    output1 = output1 + ']'
    f = open(file_name[cur_file] + '_neg_vector.txt', 'w')
    f.write(output1)
    f.close()

    output2 = ''
    output2 = output2 + '[ '
    pos_word_cloud = []
    for key, value in pos_words.items():
        output2 = output2 + key + ", "
        for i in range(value):
            pos_word_cloud.append(key)
    random.shuffle(pos_word_cloud)
    wc_english((',').join(pos_word_cloud), file_name[cur_file]+'_pos_wordcloud.png')
    output2 = output2 + ']'
    f = open(file_name[cur_file] + '_pos_vector.txt', 'w')
    f.write(output2)
    f.close()