#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include "windows_for_linux.h"
#include "TouchNode.h"


#define CONVERT_X(x, img) ((float)img->width / IMG_WIDTH)*x
#define CONVERT_Y(y, img) ((float)img->height / IMG_HEIGHT)*y

std::vector<TouchNode*> nodes;

IplImage* ResizeImage(IplImage* img, CvSize size) {

    IplImage* resized = cvCreateImage(size, IPL_DEPTH_8U, 3);

    cvResize(img, resized, CV_INTER_LANCZOS4);
    cvReleaseImage(&img);

    return resized;

}

IplImage* CutImage(IplImage* img, CvRect cut_rect, bool isDelete = true) {

    auto src = cvGetImageROI(img);
    cvSetImageROI(img, cut_rect);

    CvSize size = cvGetSize(img);
    IplImage* _img = cvCreateImage(size, IPL_DEPTH_8U, 3);

    cvCopy(img, _img);

    if(isDelete)
        cvReleaseImage(&img);
    else
        cvSetImageROI(img, src);

    return _img;

}

cv::Mat gammaCorrection(const cv::Mat& img, const double gamma_) {

    CV_Assert(gamma_ >= 0);
    //![changing-contrast-brightness-gamma-correction]
    cv::Mat lookUpTable(1, 256, CV_8U);
    uchar* p = lookUpTable.ptr();
    for (int i = 0; i < 256; ++i)
        p[i] = cv::saturate_cast<uchar>(pow(i / 255.0, gamma_) * 255.0);

    cv::Mat res = img.clone();
    cv::LUT(img, lookUpTable, res);
    //![changing-contrast-brightness-gamma-correction]

    return res;
}

CvScalar GetBrightness(IplImage* src) {

    if (src == nullptr) return CvScalar();


    float r = 0, g = 0, b = 0;
    int count = 0;

    for(int x = 0; x < src->width; x++) {
        for(int y = 0; y < src->height; y++) {
            count++;
            CvScalar src_bgr = cvGet2D(src, y, x);

            r += src_bgr.val[2];
            g += src_bgr.val[1];
            b += src_bgr.val[0];

        }
    }

    return cvScalar(b/count, g/count, r/count);

}

IplImage* GetInSourceMarker(IplImage* marker, IplImage* _src, float match) {
    if(_src == nullptr) {
        return nullptr;
    }

    IplImage* bg = (IplImage*)cvClone(_src);



    double min, max;
    CvPoint position;
    IplImage* c = cvCreateImage(cvSize(bg->width - marker->width + 1, bg->height - marker->height + 1), IPL_DEPTH_32F, 1);

    std::vector<CvPoint> points;

    cvShowImage("sample", bg);
    cvWaitKey(1);

    for (int i = 0; i < 2; i++) {


        cvMatchTemplate(bg, marker, c, CV_TM_CCORR_NORMED);
        cvMinMaxLoc(c, &min, &max, nullptr, &position);
        std::cout << "min : " << min << ", max : " << max << ", x = " << position.x << ", y = " << position.y << std::endl;

        if (max < match) return nullptr;
        points.push_back(CvPoint(position));

//        cvRectangle(bg, position, cvPoint(position.x + marker->width, position.y + marker->height), CV_RGB(255, 0, 0), 4);


        CvRect rect = cvRect(position.x, position.y, marker->width + 1, marker->height + 1);
        IplImage* result = CutImage(bg, rect, false);


        cvShowImage("sample", result);
        auto key = cvWaitKey();
        if(key == 1048603) {
            cvReleaseImage(&result);
            continue;
        }

        cvReleaseImage(&bg);
        cvReleaseImage(&c);
        return result;
    }

    cvReleaseImage(&c);
    cvReleaseImage(&bg);

    return nullptr;
}

std::vector<CvPoint> FindImages(IplImage* marker, IplImage* _src, float match, int count, float brightness) {

    if(_src == nullptr) {
        return std::vector<CvPoint>();
    }

    IplImage* bg = (IplImage*)cvClone(_src);

//    if(!disableScale) {
//        CvSize size = cvSize(CONVERT_X(objectImg->width, src), CONVERT_Y(objectImg->height, src));
//        RESIZE_IMAGE(objectImg, size);
//    }

//    if (cropSrc.height > 0 && cropSrc.width > 0) {
//        CUT_IMAGE(src, cropSrc);
//        //cvShowImage("sample", src);
//    }

    //cvShowImage("이진", src);
    //cvWaitKey(1);

//    cvShowImage("sample", bg);
//    cvWaitKey(1);

    bool isJudged = false;
    double min, max;
    CvPoint position;
    IplImage* c = cvCreateImage(cvSize(bg->width - marker->width + 1, bg->height - marker->height + 1), IPL_DEPTH_32F, 1);

    std::vector<CvPoint> points;

    for (int i = 0; i < count; i++) {


        cvMatchTemplate(bg, marker, c, CV_TM_CCORR_NORMED);
        cvMinMaxLoc(c, &min, &max, nullptr, &position);
        if (max < match) break;
//        std::cout << "min : " << min << ", max : " << max << ", x = " << position.x << ", y = " << position.y << std::endl;


        CvRect rect = cvRect(position.x, position.y, marker->width + 1, marker->height + 1);
        IplImage* result = CutImage(bg, rect, false);

        cvRectangle(bg, position, cvPoint(position.x + marker->width, position.y + marker->height), CV_RGB(255, 0, 0), 4);

        {

            auto bg_brightness = GetBrightness(result);
            float brightness_src = (bg_brightness.val[2] + bg_brightness.val[1] + bg_brightness.val[0])/3.f;

//            std::cout << "bright = " << brightness_src << "\n";

            if(abs(brightness - brightness_src) < 30) {
                cvRectangle(bg, position, cvPoint(position.x + marker->width, position.y + marker->height), CV_RGB(0, 0, 255), CV_FILLED);
                isJudged = true;
                points.push_back(CvPoint(position));
            }

//            str = std::to_string((brightness.val[2] + brightness.val[1] + brightness.val[0])/3.f);
//            cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 1, 1, 0, 2);  //이런 저런 설정.
//
//
//            cvPutText(bg, str.c_str() ,position ,&font,cvScalar(0,255,0));
        }
        cvReleaseImage(&result);
    }

    cvShowImage("sample", bg);
    if(isJudged) {
        cvWaitKey(1);
        std::cout << "combo : " << nodes.size() << '\n';
    }else
        cvWaitKey(1);


    cvReleaseImage(&bg);
    cvReleaseImage(&c);



    return points;
}

void MakeJMTFile(std::string name) {
    std::ofstream out(name);

    if(out.is_open()) {
        for(auto node : nodes) {
            out << "#" << (node->getNum()+1) << "." << node->getTime() << '\n';
        }
    }

    out.close();
}

int main() {
    std::cout << "Hello, World!" << std::endl;

    const char* fileName = "../resources/test/zz.mp4";
    //동영상 파일로부터 부터 데이터 읽어오기 위해 준비
    cv::VideoCapture cap1(fileName);
    if (!cap1.isOpened())
    {
        printf("동영상 파일을 열수 없습니다. \n");
    }



    //동영상 플레이시 크기를  320x240으로 지정
//    cap1.set(cv::CAP_PROP_FRAME_WIDTH,320);
//    cap1.set(cv::CAP_PROP_FRAME_HEIGHT,240);

    bool isBlueShutter = false;
    int syncTime = 122;


    int width = cap1.get(cv::CAP_PROP_FRAME_WIDTH);
    int height = cap1.get(cv::CAP_PROP_FRAME_HEIGHT);
    int frame = cap1.get(CV_CAP_PROP_FPS);
    float frameTime = 1000.f / frame;
    std::cout << "size : " << width << "x" << height << ", frame : " << frame << " (per " << frameTime << " milliseconds.)" << std::endl;

    IplImage* marker = cvLoadImage("../resources/marker/shutter.png", CV_LOAD_IMAGE_COLOR);
    IplImage* marker_blue = (isBlueShutter) ? cvLoadImage("../resources/marker/shutter_blue.png", CV_LOAD_IMAGE_COLOR) : nullptr;
    bool isLocalMarker[2] = {false, !isBlueShutter};
    CvSize marker_size = cvSize(height / 4 - 15, height / 4 - 15);
    int marker_range = marker_size.height / 2;
    marker = ResizeImage(marker, marker_size);
    if(isBlueShutter) marker_blue = ResizeImage(marker_blue, marker_size);

    cv::Mat frame1;
    cv::namedWindow("video", 1);

    std::vector<float> nodePosX;
    std::vector<float> nodePosY;


    float markerBrightness[2] = {255};

    auto startTime = timeGetTime();
    int totalFrame = cap1.get(CV_CAP_PROP_FRAME_COUNT);

    for (;;)
    {
        //웹캡으로부터 한 프레임을 읽어옴
        cap1 >> frame1;
        frame1 = gammaCorrection(frame1, 1.06f);

        cv::imshow("video", frame1);
        IplImage src = frame1;
        IplImage* _src = &src;

        if(!isLocalMarker[0] || !isLocalMarker[1]) {

            IplImage* result = nullptr;
            int index = 0;

            if(!isLocalMarker[0]) {
                result = GetInSourceMarker(marker, _src, 0.92f);
                index = 0;
            }else if(!isLocalMarker[1]) {
                result = GetInSourceMarker(marker_blue, _src, 0.92f);
                index = 1;
            }


            if(result != nullptr) {
                cap1.set(cv::CAP_PROP_POS_FRAMES,0);

                auto brightness = GetBrightness(result);
                markerBrightness[index] = (brightness.val[2] + brightness.val[1] + brightness.val[0])/3.f;
                std::cout << "bright = " << markerBrightness[index] << "\n";

                if(index == 0) {
                    cvReleaseImage(&marker);
                    marker = result;
                }else {
                    cvReleaseImage(&marker_blue);
                    marker_blue = result;
                }
                isLocalMarker[index] = true;
            }

            continue;
        }

        long deltaTime = cap1.get(cv::CAP_PROP_POS_FRAMES) * frameTime;
//        long deltaTime = timeGetTime() - startTime;
//        int deltaFrame = deltaTime / frameTime;
//        cap1.set(cv::CAP_PROP_POS_FRAMES,deltaFrame);

        auto points = FindImages(marker, _src, 0.96f, 16, markerBrightness[0]);
        if(isBlueShutter) {
            auto points_blue = FindImages(marker_blue, _src, 0.96f, 16, markerBrightness[1]);
            points.insert(points.end(), points_blue.begin(), points_blue.end());
        }

        if(!points.empty()) {
            for(auto point : points) {
                TouchNode* node = new TouchNode();
                node->SetPosition(point.x, point.y);
                node->setTime(deltaTime + syncTime);

                for(int i = 0; i < nodePosX.size(); i++) {
                    if(abs(point.x - nodePosX[i]) <= marker_range) {
                        break;
                    }
                    if(i == nodePosX.size() - 1) {
                        nodePosX.push_back(point.x);
                    }
                }

                for(int i = 0; i < nodePosY.size(); i++) {
                    if(abs(point.y - nodePosY[i]) <= marker_range) {
                        break;
                    }
                    if(i == nodePosY.size() - 1) {
                        nodePosY.push_back(point.y);
                    }
                }

                if(nodePosX.empty()) {
                    nodePosX.push_back(point.x);
                    nodePosY.push_back(point.y);
                }

                nodes.push_back(node);
            }
        }

        int currentPosFrame = cap1.get(cv::CAP_PROP_POS_FRAMES);
        //30ms 정도 대기하도록 해야 동영상이 너무 빨리 재생되지 않음.
        if (totalFrame - 1 <= currentPosFrame) break; //ESC키 누르면 종료
    }

    std::cout << "full combo is " << nodes.size() << "!\n";

    std::sort(nodePosX.begin(), nodePosX.end());
    std::sort(nodePosY.begin(), nodePosY.end());

    std::cout << "panel Pos = ";

    int coolTime[16] = {-9999};

    for(int i = 0; i < nodePosX.size(); i++) {
        std::cout << "(" << nodePosX[i] << ", " << nodePosY[i] << ") ";
    }

    std::cout << std::endl << "Setting Panel Position...";

    for(int count = 0; count < nodes.size(); count++) {
        TouchNode* node = nodes[count];
        int x = node->GetX();
        int y = node->GetY();

        int x_id, y_id;

        for(int i = 0; i < nodePosX.size(); i++) {
            if(abs(x - nodePosX[i]) <= marker_range) {
                x_id = i;
                break;
            }
        }

        for(int i = 0; i < nodePosY.size(); i++) {
            if(abs(y - nodePosY[i]) <= marker_range) {
                y_id = i;
                break;
            }
        }

        node->setNum(y_id * 4 + x_id);

        //중복 제거
        if(node->getTime() - coolTime[node->getNum()] < 150) {
            delete node;
            nodes.erase(nodes.begin() + count);
            count--;
            continue;
        }
        coolTime[node->getNum()] = node->getTime();

        std::cout << "[" << node->getNum() + 1 << "] " << node->getTime() << "ms (" << (count + 1) << ")\n";
    }

    std::cout << "[renew] full combo is " << nodes.size() << "!\n";
    MakeJMTFile("../resources/result/zz/pt_ext.jmt");

    cap1.set(cv::CAP_PROP_POS_FRAMES,0);
    cvShowImage("sample", marker);
    cvWaitKey();

    startTime = timeGetTime();
    bool isStart = false;


    for(int index = 0;;) {
        if(!isStart) {
            if ( cv::waitKey(100) == 13 ) {
                isStart = true;
                startTime = timeGetTime();
            }
            continue;
        }
        long deltaTime = timeGetTime() - startTime;
        int deltaFrame = deltaTime / frameTime;
        cap1.set(cv::CAP_PROP_POS_FRAMES,deltaFrame);
        cap1 >> frame1;



        for(int i = index; i < nodes.size(); i++) {
            TouchNode* node = nodes[i];
            int t = node->getTime();
            if(deltaTime - t > 400) {index++; continue;}
            if(t - deltaTime > 0) break;

            CvPoint position = cvPoint(node->GetX(), node->GetY());

            cv::rectangle(frame1, position, cvPoint(position.x + marker->width, position.y + marker->height), CV_RGB(0, 0, 255), 4);


            std::string str;

            str = "[" + std::to_string(node->getNum()) + "] " + std::to_string(i + 1);


            cv::putText(frame1, str.c_str() ,cvPoint(position.x, position.y + marker->height) ,CV_FONT_HERSHEY_SIMPLEX, 0.5, cvScalar(0,255,0), 2);
        }

        cv::imshow("video", frame1);
//        cvReleaseImage(&bg);
        if ( cv::waitKey(10) == 27 ) break; //ESC키 누르면 종료
        int currentPosFrame = cap1.get(cv::CAP_PROP_POS_FRAMES);
        //30ms 정도 대기하도록 해야 동영상이 너무 빨리 재생되지 않음.
        if (totalFrame - 1 <= currentPosFrame) break; //ESC키 누르면 종료
    }

    for(auto node : nodes) {
        delete node;
    }

    cvReleaseImage(&marker);

    return 0;
}