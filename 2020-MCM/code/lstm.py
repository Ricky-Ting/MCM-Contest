#Importing libraries
import numpy as np 
import pandas as pd
from sklearn.feature_extraction.text import CountVectorizer
from keras.preprocessing.text import Tokenizer
from keras.preprocessing.sequence import pad_sequences
from keras.models import Sequential
from keras.layers import Dense, Embedding, LSTM
from sklearn.model_selection import train_test_split
from keras.utils.np_utils import to_categorical
import re
import pandas as pd

file_name = ['hair_dryer', 'pacifier', 'microwave']
must_contain = ['dryer', 'pacifier', 'microwave']
cur_file = 1

def get_csv(filename, must_contain):
    data = pd.read_csv(filename + '.tsv', sep='\t', header=0)
    data = data[data['product_title'].str.contains(must_contain)]
    return data

def parse_year(date):
    day_s, mon_s, year_s = (date).split('/')
    return int(year_s)

def get_drop(data):
    ret = []
    for index, row in data.iterrows():
        if row.star_rating>1 and row.star_rating<5:
            ret.append(index)
    return ret


epoch = 4

#Converting into pandas dataframe and filtering only text and ratings given by the users
df = get_csv(file_name[cur_file], must_contain[cur_file])
data = df.copy()
data = data.drop(get_drop(data))
print(len(df))
print(len(data))
data = data[['review_body', 'star_rating']]

#I have considered a rating above 3 as positive and less than or equal to 3 as negative.

data['sentiment']=['pos' if (x>3) else 'neg' for x in data['star_rating']]
data['review_body'] = data['review_body'].apply(lambda x: str(x))
data['review_body'] = data['review_body'].apply((lambda x: re.sub('[^a-zA-z0-9\s]','',x)))
data['review_body'] = data['review_body'].apply(lambda x: x.lower())
for idx,row in data.iterrows():
    row[0] = row[0].replace('rt',' ')
#data['review_body']= [x.encode('ascii') for x in data['review_body']]

tokenizer = Tokenizer(nb_words = 2500, split=' ')
tokenizer.fit_on_texts(data['review_body'].values)
#print(tokenizer.word_index)  # To see the dicstionary
X = tokenizer.texts_to_sequences(data['review_body'].values)
X = pad_sequences(X)

embed_dim = 128
lstm_out = 300
batch_size= 32

##Buidling the LSTM network

model = Sequential()
model.add(Embedding(2500, embed_dim,input_length = X.shape[1], dropout=0.1))
model.add(LSTM(lstm_out, dropout_U=0.1, dropout_W=0.1))
model.add(Dense(2,activation='softmax'))
model.compile(loss = 'categorical_crossentropy', optimizer='adam',metrics = ['accuracy'])

Y = pd.get_dummies(data['sentiment']).values
X_train, X_valid, Y_train, Y_valid = train_test_split(X,Y, test_size = 0.30, random_state = 36)

#Here we train the Network.

model.fit(X_train, Y_train, batch_size =batch_size, nb_epoch = epoch,  verbose = 5)

# Measuring score and accuracy on validation set

score,acc = model.evaluate(X_valid, Y_valid, verbose = 2, batch_size = batch_size)
print("Logloss score: %.2f" % (score))
print("Validation set Accuracy: %.2f" % (acc))


df['sentiment']=['pos' if (x>3) else 'neg' for x in df['star_rating']]
df['review_body'] = df['review_body'].apply(lambda x: str(x))
df['review_body'] = df['review_body'].apply((lambda x: re.sub('[^a-zA-z0-9\s]','',x)))
df['review_body'] = df['review_body'].apply(lambda x: x.lower())
for idx,row in df.iterrows():
    row[0] = row[0].replace('rt',' ')
#data['review_body']= [x.encode('ascii') for x in data['review_body']]

#print(tokenizer.word_index)  # To see the dicstionary
newX = tokenizer.texts_to_sequences(df['review_body'].values)
newX = pad_sequences(newX)

newY = model.predict(newX)

print(newY)

newY = np.array(newY)

newY = newY[:, 1]

df['sentiment'] = newY

df.to_csv(file_name[cur_file] + '_epoch=' + str(epoch) +'_sentiment.csv')

