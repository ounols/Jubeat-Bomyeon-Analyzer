#include <iostream>
#include <opencv2/opencv.hpp>

int main() {
    std::cout << "Hello, World!" << std::endl;

    const char* fileName = "../resources/test/1116.mp4";
    //동영상 파일로부터 부터 데이터 읽어오기 위해 준비
    cv::VideoCapture cap1(fileName);
    if (!cap1.isOpened())
    {
        printf("동영상 파일을 열수 없습니다. \n");
    }

    //동영상 플레이시 크기를  320x240으로 지정
//    cap1.set(cv::CAP_PROP_FRAME_WIDTH,320);
//    cap1.set(cv::CAP_PROP_FRAME_HEIGHT,240);


    cv::Mat frame1;
    cv::namedWindow("video", 1);



    for (;;)
    {

        //웹캡으로부터 한 프레임을 읽어옴
        cap1 >> frame1;

        cv::imshow("video", frame1);


        //30ms 정도 대기하도록 해야 동영상이 너무 빨리 재생되지 않음.
        if ( cv::waitKey(20) == 27 ) break; //ESC키 누르면 종료
    }

    return 0;
}