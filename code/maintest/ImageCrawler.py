#-*- coding: utf-8 -*-
from bs4 import BeautifulSoup
import urllib.request

class crawlerImageDownload:
    def imageDownlad(self, imageUrl,count,imgFormat):
        image = urllib.request.urlopen(imageUrl)

        fileName = 'image/image_7'+ str(count) + imgFormat
        imageFile = open(fileName, 'wb')
        imageFile.write(image.read())
        imageFile.close()

if __name__ == '__main__':
    # Crawling URL
    CRAWLING_URL = 'http://www.loveloveme.com/'

    # 지정된 URL을 오픈하여 requset 정보를 가져옵니다
    source_code_from_URL = urllib.request.urlopen(CRAWLING_URL)

    soup = BeautifulSoup(source_code_from_URL, 'html.parser')
    
    index = 0

    for item in soup.find_all('img'):
        #print(item)
        #print(item['src'])
        src = str(item['src'])
        #print(src)
        #if(src.find('facebook')):
        #    print(src)
        if(src[0:11] == 'about:blank'):
            continue
        if(src[0:5] == 'https'):
            continue
        elif (src[0:7] == 'http://'):
            src = src
        elif (src[0:2] == '//'):
            src = 'http:'+src
        elif (src[0:1] == '/'):
            src = CRAWLING_URL+src

        if(src.find('gif') != -1):
            continue
        elif(src.rfind('png') != -1):    
            print(src)
            cid = crawlerImageDownload()
            cid.imageDownlad(src, index,'.png')
        elif(src.rfind('jpg') != -1):
            print(src)
            cid = crawlerImageDownload()
            cid.imageDownlad(src, index,'.jpg')
        index += 1

