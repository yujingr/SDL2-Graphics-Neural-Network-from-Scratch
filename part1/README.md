# Final Project - Computer Graphics

<img src="./samples/Screen Shot 2020-12-06 at 6.43.22 AM.png">

## Description

This is an openGL rendering of a neural network, using a dense architecture. The neural network recognizes user inputs from a drawing board that is clickable. And it updates everytime the board changes.

The neural network is trained on the MNIST dataset that has been scaled down and converted to binary.
* Code for the Notebook [https://colab.research.google.com/drive/1BFWMxlakhtW39AmvQ-Sk6ZqQI8keMrNo?usp=sharing]
  * a copy of the notebook is also included in the project folder
  * a .py version is also included

## Features
* Colors - Red for zero/negatives, Green for positves.
* Updates - calculations for each layer is calculated quickly; this affects nodes' sizes and colors, lines' colors, and output sizes.
* KEYS
  * C for clearing the board
  * Q for quitting the program
  * WERT for updating the layers individually.
    * Note: must comment out first four lines under SDLGraphicsProgram::update() to enable manual key updates.

## Going further
* Instead of Dense Model, use a Convolutional Model for higher accuracy.
* Optimizing for faster loading and rendering
