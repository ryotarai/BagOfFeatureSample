#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QFileDialog"
#include "QDateTime"
#include "QDebug"
#include "debugmat.h"

#include <opencv2/nonfree/features2d.hpp>


const int BOW_CLUSTER_COUNT = 16;
const int KMEANS_CLUSTER_COUNT = 3;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_trainingPathButton_clicked()
{
    QFileDialog qfd;
    QString dir = qfd.getExistingDirectory(this, tr("Select training image directory"));
    this->ui->trainingPathLabel->setText(dir);
}

void MainWindow::on_targetPathButton_clicked()
{
    QFileDialog qfd;
    QString dir = qfd.getExistingDirectory(this, tr("Select target image directory"));
    this->ui->targetPathLabel->setText(dir);
}

void MainWindow::on_outputPathButton_clicked()
{
    QFileDialog qfd;
    QString dir = qfd.getExistingDirectory(this, tr("Select output directory"));
    this->ui->outputPathLabel->setText(dir);
}

void MainWindow::readImages(QString &dir, vector<Mat> &imgs)
{
    QDir qdir(dir);
    readImages(qdir, imgs);
}

void MainWindow::readImages(QDir &dir, vector<Mat> &imgs)
{
    imgs.clear();

    QStringList image_name_filter;
    image_name_filter << "*.png" << "*.jpg";
    QStringList entries = dir.entryList(image_name_filter);

    QStringList::Iterator it = entries.begin();
    for (; it != entries.end(); it++) {
        QString filename = *it;
        QString filepath = dir.filePath(filename);

        Mat img = imread(filepath.toStdString());
        imgs.push_back(img);
    }
}


void MainWindow::train(BOWTrainer &trainer, const vector<Mat> trainer_imgs)
{
    vector<Mat>::const_iterator it = trainer_imgs.begin();
    int i = 0;
    for (; it != trainer_imgs.end(); it++) {
        qDebug() << "adding descriptor to trainer... " << i + 1;

        Mat img = *it;
        Mat descriptors;
        getDescriptors(img, descriptors);

        if (descriptors.data == NULL) continue;

        trainer.add(descriptors);

        i++;
    }
}

void MainWindow::getDescriptors(const Mat &img, Mat &descriptors)
{
    SiftFeatureDetector detector;

    vector<KeyPoint> keypoints;
    detector.detect(img, keypoints);

    SiftDescriptorExtractor extractor;
    extractor.compute(img, keypoints, descriptors);
}

void MainWindow::extractDescriptor(const Mat &vocabulary, const vector<Mat> target_imgs, Mat &descriptors)
{
    // create detector
    SiftFeatureDetector detector;

    // create extractor
    Ptr<DescriptorExtractor> sift(new SiftDescriptorExtractor());
    Ptr<DescriptorMatcher> matcher(new FlannBasedMatcher());
    BOWImgDescriptorExtractor extractor(sift, matcher);
    extractor.setVocabulary(vocabulary);

    // initialize mat
    descriptors.create(target_imgs.size(), BOW_CLUSTER_COUNT, CV_32FC1);

    vector<Mat>::const_iterator it = target_imgs.begin();
    int i = 0;
    for (; it != target_imgs.end(); it++) {
        qDebug() << "extracting descriptor... " << i + 1;

        Mat img = *it;

        // detect keypoints
        vector<KeyPoint> keypoints;
        detector.detect(img, keypoints);

        // compute descriptors of the keypoints
        Mat descriptor;
        extractor.compute(img, keypoints, descriptor);

        // create ROI and copyto
        Rect roi_rect;
        roi_rect.width = descriptor.cols;
        roi_rect.height = descriptor.rows;
        roi_rect.x = 0;
        roi_rect.y = i;

        Mat roi(descriptors, roi_rect);
        descriptor.copyTo(roi);

        i++;
    }
}


void MainWindow::saveKmeansResult(const Mat &labels, const vector<Mat> &images, const QString &saveDir, const int cluster_count)
{
    // create dir
    QDir dir(saveDir);
    QDateTime now = QDateTime::currentDateTime();
    QString now_str = now.toString("yyyyMMdd_hhmmss");
    dir.mkdir(now_str);
    dir.cd(now_str);
    for (int i = 0; i < cluster_count; i++) {
        dir.mkdir(QString::number(i));
    }

    for (int i = 0; i < labels.rows; i++) {
        int label = labels.at<int>(i, 0);
        QDir d(dir);
        d.cd(QString::number(label));
        QString filename = d.filePath(QString::number(rand()) + ".jpg");
        imwrite(filename.toStdString(), images[i]);
    }
}



void MainWindow::on_executeButton_clicked()
{
    BOWKMeansTrainer trainer(BOW_CLUSTER_COUNT);

    // 画像読み込み
    QString trainer_path = this->ui->trainingPathLabel->text();
    vector<Mat> trainer_imgs;
    readImages(trainer_path, trainer_imgs);

    // Bag of featuresでクラスタ中心を求める
    train(trainer, trainer_imgs);

    // trainer_imgsを開放
    trainer_imgs.clear();

    // clustering
    qDebug() << "clustering...";
    Mat cluster = trainer.cluster();

    // descriptor算出
    // BOWImgDescriptorExtractor
    QString target_path = this->ui->targetPathLabel->text();
    vector<Mat> target_imgs;
    readImages(target_path, target_imgs);
    Mat descriptors;

    extractDescriptor(cluster,
                      target_imgs,
                      descriptors);
    /*
      descriptorsの1行目 <-> target_imgs[0]
      descriptorsの2行目 <-> target_imgs[1]
      ...
      と対応している
    */

    // 算出したdescriptorをk-means
    Mat labels, center_of_clusters;
    int max_iteration_count = 128;
    TermCriteria termcrit(static_cast<int>(TermCriteria::MAX_ITER), max_iteration_count, 0.0/* no meaning */);
    int attempts = 16; // 再配置回数

    kmeans(descriptors, KMEANS_CLUSTER_COUNT, labels, termcrit, attempts, KMEANS_PP_CENTERS, center_of_clusters);

    cv_for_qt::writeMatToQDebug(labels);


    QString output_path = this->ui->outputPathLabel->text();
    saveKmeansResult(labels, target_imgs, output_path, KMEANS_CLUSTER_COUNT);

}

