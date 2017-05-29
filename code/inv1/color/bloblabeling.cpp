#include "bloblabeling.h"
#include <iostream>

/// @brief CBlobLabeling ������
/// @remarks Labeling�� ���Ǵ� ���� ���� �ʱ�ȭ
CBlobLabeling::CBlobLabeling(void)
{
    m_nThreshold    = 0;
    m_nBlobs        = _DEF_MAX_BLOBS;
    m_Image         = NULL;

    m_vPoint        = NULL;

    m_recBlobs      = NULL;
    m_intBlobs      = NULL;
}

/// @brief CBlobLabeling �Ҹ���
/// @remarks Labeling�� ���� ���� ���� ����
CBlobLabeling::~CBlobLabeling(void)
{
    if( m_Image != NULL )
    {
        delete m_Image;
        m_Image = NULL;
    }

    if( m_recBlobs != NULL )
    {
        delete m_recBlobs;
        m_recBlobs = NULL;
    }

    if( m_intBlobs != NULL )
    {
        delete m_intBlobs;
        m_intBlobs = NULL;
    }
}

/// @brief Parameter ����
/// @remarks Labeling�� ���Ǵ� �̹����� ���� ũ�� ������ ����
/// @param[in] image        Labeling�� ����� �̹���. 0 �Ǵ� -1�� ���� ������ 1ch �̹���.
/// @param[in] nThreshold   Labeling ��� �� Blob�� �ȼ� ���� nThreshold ������ ��� ����
bool CBlobLabeling::SetParam(IplImage* image, int nThreshold)
{
    if( image->nChannels != 1 ) 	return false;

    if( m_recBlobs != NULL )
    {
        delete m_recBlobs;
        m_recBlobs	= NULL;
    }

    if( m_intBlobs != NULL )
    {
        delete m_intBlobs;
        m_intBlobs	= NULL;
    }

    if( m_Image != NULL )
    {
        delete m_Image;
        m_Image = NULL;
    }

    m_nWidth = image->width;
    m_nHeight = image->height;

    m_Image = new int [ m_nWidth * m_nHeight ];

    int i,j;

    for(j=0;j<m_nHeight;j++)
    for(i=0;i<m_nWidth ;i++)
    {
        // object�� -1�� üũ
//        unsigned char val = (unsigned char)image->imageData[j*image->widthStep+i];
        unsigned char val = (unsigned char)cvGet2D( image, j, i ).val[0];
        m_Image[j*m_nWidth+i] = ( val == 0 ? 0 : -1 );
    }

    m_nThreshold	= nThreshold;
    m_nBlobs        = _DEF_MAX_BLOBS;

    return true;
}

/// @brief Labeling ����
void CBlobLabeling::DoLabeling()
{
    m_nBlobs = Labeling(m_Image, m_nWidth, m_nHeight, m_nThreshold);
}

/// @brief Labeling ����
/// @param[in] image        Labeling�� ����� �̹���. 0 �Ǵ� -1�� ���� ������ 1ch �̹���.
/// @param[in] nThreshold   Labeling ��� �� Blob�� �ȼ� ���� nThreshold ������ ��� ����
/// @return Label ����
int CBlobLabeling::Labeling(int* DataBuf, int nWidth, int nHeight, int nThreshold)
{
    int nNumber;

    // ���̺����� ���� ����Ʈ �ʱ�ȭ
    InitvPoint(nWidth, nHeight);

    // ���̺���
    nNumber = _Labeling(DataBuf, nWidth, nHeight, nThreshold);

    // ����Ʈ �޸� ����
    DeletevPoint();

    if( nNumber != 0 )
    {
        m_recBlobs = new CvRect [nNumber];
        m_intBlobs = new int [nNumber];

        DetectLabelingRegion(nNumber, DataBuf, nWidth, nHeight);
    }

	//std::cout << nNumber << std::endl;

    return nNumber;
}

/// @brief m_vPoint �ʱ�ȭ
void CBlobLabeling::InitvPoint(int nWidth, int nHeight)
{
    int nX, nY;

    m_vPoint = new Visited [nWidth * nHeight];

    for(nY = 0; nY < nHeight; nY++)
    {
        for(nX = 0; nX < nWidth; nX++)
        {
            m_vPoint[nY * nWidth + nX].bVisitedFlag		= false;
            m_vPoint[nY * nWidth + nX].ptReturnPoint.x	= nX;
            m_vPoint[nY * nWidth + nX].ptReturnPoint.y	= nY;
        }
    }
}

/// @brief m_vPoint ����
void CBlobLabeling::DeletevPoint()
{
    delete m_vPoint;
    m_vPoint = NULL;
}

/// @brief Labeling ���� �Լ�
/// @remarks Size�� nWidth�̰� nHeight�� DataBuf���� nThreshold���� ���� ������ ������ �������� blob���� ȹ��
/// @param[in] DataBuf      Labeling�� ����� �̹��� ������ ����. 0 �Ǵ� -1�� ���� ����.
/// @param[in] nWidth       ������ ������ ���� ����
/// @param[in] nHeight      ������ ������ ���� ����
/// @param[in] nThreshold   Labeling ��� �� Blob�� �ȼ� ���� nThreshold ������ ��� ����
/// @return Label ����
int CBlobLabeling::_Labeling(int *DataBuf, int nWidth, int nHeight, int nThreshold)
{
    int num = 0;
    int nX, nY, k, l;
    int StartX , StartY, EndX , EndY;

    // Find connected components
    for(nY = 0; nY < nHeight; nY++)
    {
        for(nX = 0; nX < nWidth; nX++)
        {
            if( DataBuf[nY * nWidth + nX] == -1 )    // Is this a new component?
            {
                num++;

                DataBuf[nY * nWidth + nX] = num;

                StartX = nX, StartY = nY, EndX = nX, EndY= nY;

                __NRFIndNeighbor(DataBuf, nWidth, nHeight, nX, nY, &StartX, &StartY, &EndX, &EndY);

                if(__Area(DataBuf, StartX, StartY, EndX, EndY, nWidth, num) < nThreshold)
                {
                    for(k = StartY; k <= EndY; k++)
                    {
                        for(l = StartX; l <= EndX; l++)
                        {
                            if(DataBuf[k * nWidth + l] == num)
                                DataBuf[k * nWidth + l] = 0;
                        }
                    }
                    --num;

                    if(num > _DEF_MAX_BLOBS)    return  _DEF_MAX_BLOBS;
                }
            }
        }
    }

    return num;
}

/// @brief Labeling ����� Blob ������ ���ϴ� �Լ�
/// @param[in] nLabelNumber �ش� Blob�� Label
/// @param[in] DataBuf      Labeling Result
/// @param[in] nWidth       DataBuf ���� ũ��
/// @param[in] nHeight      DataBuf ���� ũ��
void CBlobLabeling::DetectLabelingRegion(int nLabelNumber, int* DataBuf, int nWidth, int nHeight)
{
    int nX, nY;
    int nLabelIndex ;

	bool* bFirstFlag = new bool[nLabelNumber];
    for(int b=0;b<nLabelNumber;b++) bFirstFlag[b] = false;

    for(nY = 0; nY < nHeight; nY++)
    {
        for(nX = 0; nX < nWidth; nX++)
        {
            nLabelIndex = DataBuf[nY * nWidth + nX];

            if(nLabelIndex > 0)	// Is this a new component?
            {
                if(bFirstFlag[nLabelIndex-1] == false)
                {
                    m_recBlobs[nLabelIndex-1].x         = nX;
                    m_recBlobs[nLabelIndex-1].y     	= nY;
                    m_recBlobs[nLabelIndex-1].width 	= 0;
                    m_recBlobs[nLabelIndex-1].height	= 0;

                    bFirstFlag[nLabelIndex-1] = true;
                }
                else
                {
                    int left	= m_recBlobs[nLabelIndex-1].x;
                    int right	= left + m_recBlobs[nLabelIndex-1].width;
                    int top     = m_recBlobs[nLabelIndex-1].y;
                    int bottom	= top + m_recBlobs[nLabelIndex-1].height;

                    if( left   >= nX )	left	= nX;
                    if( right  <= nX )	right	= nX+1;
                    if( top    >= nY )	top 	= nY;
                    if( bottom <= nY )	bottom	= nY+1;

                    m_recBlobs[nLabelIndex-1].x     	= left;
                    m_recBlobs[nLabelIndex-1].y     	= top;
                    m_recBlobs[nLabelIndex-1].width 	= right - left;
                    m_recBlobs[nLabelIndex-1].height	= bottom - top;

                    m_intBlobs[nLabelIndex-1]       	= nLabelIndex;
                }
            }
        }
    }

    delete bFirstFlag;
}

/// @brief Blob Labeling�� ���� ���ϴ� �Լ�
/// @param[in] DataBuf   Labeling�� ����� �̹��� ������ ����. 0 �Ǵ� -1�� ���� ����.
/// @param[in] nWidth   ������ ������ ���� ����
/// @param[in] nHeight  ������ ������ ���� ����
/// @param[in] nPosX    ���� Ž������ X��ǥ
/// @param[in] nPosY    ���� Ž������ Y��ǥ
/// @param[out] StartX  Ž�� �������� X��ǥ
/// @param[out] StartY  Ž�� �������� Y��ǥ
/// @param[out] EndX    Ž�� �������� X��ǥ
/// @param[out] EndY    Ž�� �������� Y��ǥ
/// @return 0
int CBlobLabeling::__NRFIndNeighbor(int* DataBuf, int nWidth, int nHeight, int nPosX, int nPosY, int *StartX, int *StartY, int *EndX, int *EndY )
{
    CvPoint CurrentPoint;

    CurrentPoint.x = nPosX;
    CurrentPoint.y = nPosY;

    m_vPoint[CurrentPoint.y * nWidth +  CurrentPoint.x].bVisitedFlag    = true;
    m_vPoint[CurrentPoint.y * nWidth +  CurrentPoint.x].ptReturnPoint.x = nPosX;
    m_vPoint[CurrentPoint.y * nWidth +  CurrentPoint.x].ptReturnPoint.y = nPosY;

    while(1)
    {
        if( (CurrentPoint.x != 0) && (DataBuf[CurrentPoint.y * nWidth + CurrentPoint.x - 1] == -1) )   // -X ����
        {
            if( m_vPoint[CurrentPoint.y * nWidth +  CurrentPoint.x - 1].bVisitedFlag == false )
            {
                DataBuf[CurrentPoint.y  * nWidth + CurrentPoint.x  - 1]                 = DataBuf[CurrentPoint.y * nWidth + CurrentPoint.x];	// If so, mark it
                m_vPoint[CurrentPoint.y * nWidth +  CurrentPoint.x - 1].bVisitedFlag	= true;
                m_vPoint[CurrentPoint.y * nWidth +  CurrentPoint.x - 1].ptReturnPoint	= CurrentPoint;
                CurrentPoint.x--;

                if(CurrentPoint.x <= 0)
                    CurrentPoint.x = 0;

                if(*StartX >= CurrentPoint.x)
                    *StartX = CurrentPoint.x;

                continue;
            }
        }

        if( (CurrentPoint.x != nWidth - 1) && (DataBuf[CurrentPoint.y * nWidth + CurrentPoint.x + 1] == -1) )   // -X ����
        {
            if( m_vPoint[CurrentPoint.y * nWidth +  CurrentPoint.x + 1].bVisitedFlag == false )
            {
                DataBuf[CurrentPoint.y * nWidth + CurrentPoint.x + 1]       		= DataBuf[CurrentPoint.y * nWidth + CurrentPoint.x];	// If so, mark it
                m_vPoint[CurrentPoint.y * nWidth +  CurrentPoint.x + 1].bVisitedFlag	= true;
                m_vPoint[CurrentPoint.y * nWidth +  CurrentPoint.x + 1].ptReturnPoint	= CurrentPoint;
                CurrentPoint.x++;

                if(CurrentPoint.x >= nWidth - 1)
                    CurrentPoint.x = nWidth - 1;

                if(*EndX <= CurrentPoint.x)
                    *EndX = CurrentPoint.x;

                continue;
            }
        }

        if( (CurrentPoint.y != 0) && (DataBuf[(CurrentPoint.y - 1) * nWidth + CurrentPoint.x] == -1) )   // -X ����
        {
            if( m_vPoint[(CurrentPoint.y - 1) * nWidth +  CurrentPoint.x].bVisitedFlag == false )
            {
                DataBuf[(CurrentPoint.y - 1) * nWidth + CurrentPoint.x]                 = DataBuf[CurrentPoint.y * nWidth + CurrentPoint.x];	// If so, mark it
                m_vPoint[(CurrentPoint.y - 1) * nWidth +  CurrentPoint.x].bVisitedFlag	= true;
                m_vPoint[(CurrentPoint.y - 1) * nWidth +  CurrentPoint.x].ptReturnPoint = CurrentPoint;
                CurrentPoint.y--;

                if(CurrentPoint.y <= 0)
                    CurrentPoint.y = 0;

                if(*StartY >= CurrentPoint.y)
                    *StartY = CurrentPoint.y;

                continue;
            }
        }

        if( (CurrentPoint.y != nHeight - 1) && (DataBuf[(CurrentPoint.y + 1) * nWidth + CurrentPoint.x] == -1) )   // -X ����
        {
            if( m_vPoint[(CurrentPoint.y + 1) * nWidth +  CurrentPoint.x].bVisitedFlag == false )
            {
                DataBuf[(CurrentPoint.y + 1) * nWidth + CurrentPoint.x]                 = DataBuf[CurrentPoint.y * nWidth + CurrentPoint.x];	// If so, mark it
                m_vPoint[(CurrentPoint.y + 1) * nWidth +  CurrentPoint.x].bVisitedFlag	= true;
                m_vPoint[(CurrentPoint.y + 1) * nWidth +  CurrentPoint.x].ptReturnPoint = CurrentPoint;
                CurrentPoint.y++;

                if(CurrentPoint.y >= nHeight - 1)
                    CurrentPoint.y = nHeight - 1;

                if(*EndY <= CurrentPoint.y)
                    *EndY = CurrentPoint.y;

                continue;
            }
        }

        if(     (CurrentPoint.x == m_vPoint[CurrentPoint.y * nWidth + CurrentPoint.x].ptReturnPoint.x)
            &&	(CurrentPoint.y == m_vPoint[CurrentPoint.y * nWidth + CurrentPoint.x].ptReturnPoint.y) )
        {
            break;
        }
        else
        {
            CurrentPoint = m_vPoint[CurrentPoint.y * nWidth + CurrentPoint.x].ptReturnPoint;
        }
    }

    return 0;
}

/// @brief Blob ���� �� ������ Label�� ���� ������ ũ��(�ȼ� ��)�� ���ϴ� �Լ�
/// @param[in] DataBuf   Labeling�� ����� �̹��� ������ ����
/// @param[in] StartX   Ž�� �������� X��ǥ
/// @param[in] StartY   Ž�� �������� Y��ǥ
/// @param[in] EndX     Ž�� �������� X��ǥ
/// @param[in] EndY     Ž�� �������� Y��ǥ
/// @param[in] nWidth   ������ ������ ���� ����
/// @param[in] nLevel   ������ Blob�� Label
/// @return Ž������ ������ ������ Label�� �ȼ� ��
int CBlobLabeling::__Area(int* DataBuf, int StartX, int StartY, int EndX, int EndY, int nWidth, int nLevel)
{
    int nArea = 0;
    int nX, nY;

    for (nY = StartY; nY < EndY; nY++)
        for (nX = StartX; nX < EndX; nX++)
            if (DataBuf[nY * nWidth + nX] == nLevel)
                ++nArea;

	//std::cout << nArea << std::endl;

    return nArea;
}

/// @brief ����/���� ������ ������ �������� ���� Blob ����
/// @remarks Width/Hight �� fRatio���� ���� ���, �װ͵��� ������, ����� rect�� ����\n
/// Input���� ���� rect�� �װ��� ���� nRecNumber�� ������, ����� �� ��
/// @param[in] fRatio ����/���� ����
void CBlobLabeling::BlobWidthHeightSmallRatioConstraint(float fRatio)
{
    m_nBlobs = _BlobWidthHeightSmallRatioConstraint(fRatio, m_nBlobs, m_recBlobs, m_intBlobs);
}

/// @brief ����/���� ������ ������ �������� ���� Blob �����ϴ� ���� �Լ�
/// @param[in] fRatio   ����/���� ����
/// @param[in] rect     Blob ���� ����
/// @param[in] blobs    Blob Index ����
/// @param[in] nRecNumber Blob ����
/// @return ������ Blob ����
int CBlobLabeling::_BlobWidthHeightSmallRatioConstraint(float fRatio, int nRecNumber, CvRect* rect, int* blobs)
{
    if(nRecNumber == 0)	return 0;

    int nX;
    int ntempRec = 0;

    CvRect *temp = new CvRect[nRecNumber];
    int *labeled = new int[nRecNumber];

    for(nX = 0; nX < nRecNumber; nX++)
    {
        temp[nX]    = rect[nX];
        labeled[nX] = blobs[nX];
    }

    for(nX = 0; nX < nRecNumber; nX++)
    {
        if( (float)rect[nX].height / rect[nX].width > fRatio )
        {
            rect[ntempRec] = temp[nX];
            blobs[ntempRec] = labeled[nX];

            ntempRec++;
        }
    }

    delete temp;
    delete labeled;

    return ntempRec;
}

/// @brief ������ nWidth�� nHeight���� ���� Blob�� ��� ����
/// @param[in] nWidth   ���� ũ�� Threshold
/// @param[in] nHeight  ���� ũ�� Threshold
void CBlobLabeling::BlobSmallSizeConstraint(int nWidth, int nHeight)
{
    m_nBlobs = _BlobSmallSizeConstraint(nWidth, nHeight, m_nBlobs, m_recBlobs, m_intBlobs);
}

/// @brief ������ nWidth�� nHeight���� ���� Blob�� ��� �����ϴ� ���� �Լ�
/// @param[in] nWidth       ���� ũ�� Threshold
/// @param[in] nHeight      ���� ũ�� Threshold
/// @param[in] nRecNumber   Blob ����
/// @param[in] rect         Blob ���� ����
/// @param[in] blobs        Blob Index ����
/// @return ������ Blob ����
int CBlobLabeling::_BlobSmallSizeConstraint(int nWidth, int nHeight, int nRecNumber, CvRect* rect, int* label)
{
    if(nRecNumber == 0)	return 0;

    int nX;
    int ntempRec = 0;

    CvRect* temp = new CvRect[nRecNumber];
    int* labeled = new int[nRecNumber];

    for(nX = 0; nX < nRecNumber; nX++)
    {
        temp[nX]    = rect[nX];
        labeled[nX] = label[nX];
    }

    for(nX = 0; nX < nRecNumber; nX++)
    {
        if( (rect[nX].width >= nWidth) && (rect[nX].height >= nHeight) )
        {
            temp[ntempRec] = rect[nX];
            labeled[ntempRec] = label[nX];

            ntempRec++;
        }
    }

    for(nX = 0; nX < ntempRec; nX++)
    {
        rect[nX] = temp[nX];
        label[nX] = labeled[nX];
    }

    delete temp;
    delete labeled;

    return ntempRec;
}

/// @brief ������ nWidth�� nHeight���� ū Blob�� ��� ����
/// @param[in] nWidth   ���� ũ�� Threshold
/// @param[in] nHeight  ���� ũ�� Threshold
void CBlobLabeling::BlobBigSizeConstraint(int nWidth, int nHeight)
{
    m_nBlobs = _BlobBigSizeConstraint(nWidth, nHeight, m_nBlobs, m_recBlobs, m_intBlobs);
}

/// @brief ������ nWidth�� nHeight���� ū Blob�� ��� �����ϴ� ���� �Լ�
/// @param[in] nWidth       ���� ũ�� Threshold
/// @param[in] nHeight      ���� ũ�� Threshold
/// @param[in] nRecNumber   Blob ����
/// @param[in] rect         Blob ���� ����
/// @param[in] blobs        Blob Index ����
/// @return ������ Blob ����
int CBlobLabeling::_BlobBigSizeConstraint(int nWidth, int nHeight, int nRecNumber, CvRect* rect, int* label)
{
    if(nRecNumber == 0)	return 0;

    int nX;
    int ntempRec = 0;

    CvRect* temp = new CvRect [nRecNumber];
    int* labeled = new int [nRecNumber];

    for(nX = 0; nX < nRecNumber; nX++)
    {
        temp[nX] = rect[nX];
        labeled[nX] = label[nX];
    }

    for(nX = 0; nX < nRecNumber; nX++)
    {
        if( (rect[nX].width < nWidth) && (rect[nX].height < nHeight) )
        {
            temp[ntempRec] = rect[nX];
            labeled[ntempRec] = label[nX];

            ntempRec++;
        }
    }

    for(nX = 0; nX < ntempRec; nX++)
    {
        rect[nX] = temp[nX];
        label[nX] = labeled[nX];
    }

    delete temp;
    delete labeled;

    return ntempRec;
}

/// @brief �̹��� �ܰ� �κ����κ��� ������ �Ÿ� �ȿ� �ִ�(�̹��� �ܰ��� ����) Blob�� ����
/// @param[in] marginX  ���� �Ÿ� Threshold
/// @param[in] marginY  ���� �Ÿ� Threshold
void CBlobLabeling::BlobEdgeConstraint(int marginX, int marginY)
{
    m_nBlobs = _BlobEdgeConstraint(marginX, marginY, m_nBlobs, m_recBlobs, m_intBlobs);
}

/// @brief �̹��� �ܰ� �κ����κ��� ������ �Ÿ� �ȿ� �ִ� Blob�� �����ϴ� ���� �Լ�
/// @param[in] marginX      ���� �Ÿ� Threshold
/// @param[in] marginY      ���� �Ÿ� Threshold
/// @param[in] nRecNumber   Blob ����
/// @param[in] rect         Blob ���� ����
/// @param[in] blobs        Blob Index ����
/// @return ������ Blob ����
int CBlobLabeling::_BlobEdgeConstraint(int marginX, int marginY, int nRecNumber, CvRect *rect, int *label)
{
    if(nRecNumber == 0)	return 0;

    int nX;
    int ntempRec = 0;

    CvRect* temp = new CvRect [nRecNumber];
    int* labeled = new int [nRecNumber];

    for(nX = 0; nX < nRecNumber; nX++)
    {
            temp[nX] = rect[nX];
            labeled[nX] = label[nX];
    }

    for(nX = 0; nX < nRecNumber; nX++)
    {
        int l = rect[nX].x;
        int t = rect[nX].y;
        int r = rect[nX].x + rect[nX].width;
        int b = rect[nX].y + rect[nX].height;

        if(     l > marginX && r < m_nWidth - marginX
            &&  t > marginY && b < m_nHeight - marginY )
        {
                temp[ntempRec] = rect[nX];
                labeled[ntempRec] = label[nX];

                ntempRec++;
        }
    }

    for(nX = 0; nX < ntempRec; nX++)
    {
            rect[nX] = temp[nX];
            label[nX] = labeled[nX];
    }

    delete temp;
    delete labeled;

    return ntempRec;
}

/// @brief �̹����� �ѷ� �� ���� ������ ���� ��ü�� �����ϴ� ������ ������ �� ���� ���� Blob�� ����
/// @param[in] ratio ��ü�� �����ϴ� ���� ����ġ
void CBlobLabeling::BlobIncludeRatioConstraint(float ratio)
{
    m_nBlobs = _BlobIncludeRatioConstraint(ratio, m_nBlobs, m_recBlobs, m_intBlobs);
}

/// @brief �̹����� �ѷ� �� ���� ������ ���� ��ü�� �����ϴ� ������ ������ �� ���� ���� Blob�� �����ϴ� ���� �Լ�
/// @param[in] ratio        ��ü�� �����ϴ� ���� ����ġ
/// @param[in] nRecNumber   Blob ����
/// @param[in] rect         Blob ���� ����
/// @param[in] blobs        Blob Index ����
/// @return ������ Blob ����
int CBlobLabeling::_BlobIncludeRatioConstraint(float ratio, int nRecNumber, CvRect *rect, int *label)
{
    if(nRecNumber == 0)	return 0;

    int nX;
    int ntempRec = 0;

    CvRect* temp = new CvRect [nRecNumber];
    int* labeled = new int [nRecNumber];

    for(nX = 0; nX < nRecNumber; nX++)
    {
        temp[nX] = rect[nX];
        labeled[nX] = label[nX];
    }

    for(nX = 0; nX < nRecNumber; nX++)
    {
        int cnt = 0;

        for( int j = 0; j < rect[nX].height; j++ )
        for( int i = 0; i < rect[nX].width;  i++ )
        {
            unsigned char val = m_Image[ (j + rect[nX].y) * m_nWidth + (i + rect[nX].x) ];

            if( val == label[nX] )	cnt++;
        }

        float constraint = (float)cnt / (rect[nX].width * rect[nX].height);

        if( constraint > ratio )
        {
                temp[ntempRec] = rect[nX];
                labeled[ntempRec] = label[nX];

                ntempRec++;
        }
    }

    for(nX = 0; nX < ntempRec; nX++)
    {
            rect[nX] = temp[nX];
            label[nX] = labeled[nX];
    }

    delete temp;
    delete labeled;

    return ntempRec;
}

/// @brief ���ϴ� ������ �̹��� ������ �����´�
/// @remarks ���ϴ� ������ �̹����� �����ϹǷ�, �Լ� ���� ������ �̹��������� �ʱ�ȭ �Ǿ�� ��.
/// @param[out] dest    ���� �̹���
/// @param[in] nLabel   ���ϴ� Blob�� Label
/// @param[in] nX       image offset x��ǥ
/// @param[in] nY       image offset y��ǥ
void CBlobLabeling::GetBlobImage(IplImage* dest, int nLabel, int nX, int nY)
{
    CvRect rect = m_recBlobs[ nLabel ];
    int nNum	= m_intBlobs[ nLabel ];

    for( int j = 0; j < rect.height; j++ )
    for( int i = 0; i < rect.width;  i++ )
    {
        int val = m_Image[ (j + rect.y) * m_nWidth + (i + rect.x) ];

        if( val == nNum )   dest->imageData[ (nY + j) * dest->widthStep + (nX + i) ] = (unsigned char)255;
    }
}

int CBlobLabeling::result(IplImage* dest, int nBlobs)
{
	int count=0;

	for (int i = 0; i < nBlobs; i++)
	{
		CvRect rect = m_recBlobs[i];
		int nNum = m_intBlobs[i];
		
		count += m_nBlobs;

		for (int j = 0; j < rect.height; j++)
			for (int i = 0; i < rect.width; i++)
			{
				int val = m_Image[(j + rect.y) * m_nWidth + (i + rect.x)];

				if (val == nNum)   dest->imageData[(rect.y + j) * dest->widthStep + (rect.x + i)] = (unsigned char)255;
			}
	}
	return count;

}