import tensorflow as tf
from tensorflow.keras.preprocessing.image import ImageDataGenerator
from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import Dense, Flatten
from tensorflow.keras.optimizers import Adam
import numpy as np

#C du c tkt
# Repertoire des images
dataset_dir = 'Fnt'
# Faut tout resize
datagen = ImageDataGenerator(rescale=1./255, validation_split=0.2)
# charger les images 
train_generator = datagen.flow_from_directory(
    dataset_dir,
    target_size=(28, 28),
    batch_size=32,
    class_mode='categorical',
    subset='training'
)
# Validation des images
validation_generator = datagen.flow_from_directory(
    dataset_dir,
    target_size=(28, 28),
    batch_size=32,
    class_mode='categorical',
    subset='validation'
)
# Définir le modèlelll
model = Sequential([
    Flatten(input_shape=(28, 28, 3)),
    Dense(256, activation='relu'),
    Dense(26, activation='softmax')  # 26 classes for A-Z
])
# Compiler THE model
model.compile(optimizer=Adam(), loss='categorical_crossentropy', metrics=['accuracy'])
# Train THE model
model.fit(train_generator, epochs=100, validation_data=validation_generator)
# Sauver le tout
hidden_weights, hidden_biases = model.layers[1].get_weights()
output_weights, output_biases = model.layers[2].get_weights()

with open('weights.txt', 'w') as f:
    for weight in hidden_weights.flatten():
        f.write(f"{weight}\n")

with open('hidden_biases.txt', 'w') as f:
    for bias in hidden_biases:
        f.write(f"{bias}\n")

with open('output_weights.txt', 'w') as f:
    for weight in output_weights.flatten():
        f.write(f"{weight}\n")

with open('output_biases.txt', 'w') as f:
    for bias in output_biases:
        f.write(f"{bias}\n")
#TOUCHEZ PLUS A R CA FONCTIONE
