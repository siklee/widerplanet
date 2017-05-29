import tensorflow as tf
import sys
import time
import urllib2
import urllib
import argparse
from datetime import datetime
import hashlib
import os.path
import random
import re
import struct
import sys
import tarfile
import numpy as np
from six.moves import urllib
import tensorflow as tf
from tensorflow.python.framework import graph_util
from tensorflow.python.framework import tensor_shape
from tensorflow.python.platform import gfile
from tensorflow.python.util import compat

# download unsafe image file using url
def download_photo(img_url, filename, dirname):

    file_path = "%s%s" % ("./download_img/" + dirname + "/", filename[0])

    downloaded_image = file(file_path, "w")

    image_on_web = urllib2.urlopen(img_url)

    while True:
        buf = image_on_web.read(100000000)
        if len(buf) == 0:
            break
        downloaded_image.write(buf)

    downloaded_image.close()
    image_on_web.close()

    return file_path

def procecc_text(input_file):

  img_count = 0
  safe_count = 0
  unsafe_count = 0

  safe_file = open('./safe', 'w')
  unsafe_file = open('./unsafe', 'w')

  f = open(input_file, 'r')
  while True:
    input_line = f.readline()
    if not input_line: break
    
    # extract img_url
    line = input_line.split('\n')
    img_url = line[0].split('\t')
    img_url = img_url[2]

    last_char = img_url.split('.')
    last_char = last_char[-1]

    
    if  last_char != 'jpg': continue

    # change this as you see fit
    image_path = img_url

    # Read in the image_data
    
    # This line is to read image data of file_path
    """image_data = tf.gfile.FastGFile(image_path, 'rb').read()"""
    
    # This line is to read image data of URL
    try: image_data = urllib2.urlopen(image_path).read()
    except urllib2.HTTPError as err1: continue
    except urllib2.URLError as err2: continue

    img_count = img_count + 1

    with tf.Session() as sess:
        # Feed the image_data as input to the graph and get first prediction
        softmax_tensor = sess.graph.get_tensor_by_name('final_result:0')
    
        predictions = sess.run(softmax_tensor, \
                 {'DecodeJpeg/contents:0': image_data})

        # Sort to show labels of first prediction in order of confidence
        top_k = predictions[0].argsort()[::-1]

        human_string = label_lines[top_k[0]]

        if human_string == "unsafe":
            unsafe_count = unsafe_count + 1
            #download_photo(img_url, img_url.split('/')[-1:], "unsafe")
            unsafe_file.write(input_line)
            
        else:
            safe_count = safe_count + 1
            #download_photo(img_url, img_url.split('/')[-1:], "safe")
            safe_file.write(input_line)

    if img_count % 25 == 0 : print ("prosessed img count : %d " % img_count )

  print_result(img_count, safe_count, unsafe_count)
  
  f.close()
  safe_file.close()
  unsafe_file.close()

def procecc_image(image_dir):
  img_count = 0
  safe_count = 0
  unsafe_count = 0

  if not gfile.Exists(image_dir):
    print("Image directory '" + image_dir + "' not found.")
    return None

  result = {}
  extensions = ['jpg', 'jpeg', 'JPG', 'JPEG']
  file_list = []
  print("Looking for images in '" + image_dir + "'")

  for extension in extensions:
    file_glob = os.path.join(image_dir, image_dir, '*.' + extension)
    file_list.extend(gfile.Glob(file_glob))

  if not file_list:
    print('No files found')
    return 0
  
  for file_name in file_list:
    base_name = os.path.basename(file_name)
    image_data = tf.gfile.FastGFile(file_name, 'rb').read()
    
    img_count = img_count + 1

    with tf.Session() as sess:
        # Feed the image_data as input to the graph and get first prediction
      softmax_tensor = sess.graph.get_tensor_by_name('final_result:0')
    
      predictions = sess.run(softmax_tensor, \
                 {'DecodeJpeg/contents:0': image_data})

        # Sort to show labels of first prediction in order of confidence
      top_k = predictions[0].argsort()[::-1]

      human_string = label_lines[top_k[0]]

      if human_string == "unsafe":
          unsafe_count = unsafe_count + 1
          print (base_name + "  unsafe")
      else:
          safe_count = safe_count + 1
          print (base_name + "  safe")

  print_result(img_count, safe_count, unsafe_count)


def print_result(img_count, safe_count, unsafe_count):
  #print result
  print('####################################')
  print ('img_count : %d ' % img_count)
  print ('unsafe_count : %d ' % unsafe_count)
  print ('safe_count : %d ' % safe_count)

  print('\n')

  print ('safe ratio : %.2f' % (float(safe_count) / float(img_count) * 100))
  print ('unsafe ratio : %.2f' % (float(unsafe_count) / float(img_count) * 100))
  print('####################################')

#############################################
############ start main func  ###############   

#processing start time
start_time = time.time()

# Loads label file, strips off carriage return
label_lines = [line.rstrip() for line
                   in tf.gfile.GFile("retrained_labels.txt")]

# Unpersists graph from file
with tf.gfile.FastGFile("retrained_graph.pb", 'rb') as f:
    graph_def = tf.GraphDef()
    graph_def.ParseFromString(f.read())
    _ = tf.import_graph_def(graph_def, name='')

if(sys.argv[1] == "text"):  procecc_text(sys.argv[2])
elif(sys.argv[1] == "image"):  procecc_image(sys.argv[2])


#processing end time
end_time = time.time()

print ('processing time : %0.3f' % (end_time - start_time))
