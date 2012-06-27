#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDir>
#include <vector>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private slots:
    void on_trainingPathButton_clicked();

    void on_executeButton_clicked();

    void on_targetPathButton_clicked();

    void on_outputPathButton_clicked();

private:
    Ui::MainWindow *ui;

    void readImages(QString &dir, vector<Mat> &imgs);
    void readImages(QDir &dir, vector<Mat> &imgs);
    void train(BOWTrainer &trainer, const QString &dir_path);
    void getDescriptors(const Mat &img, Mat &descriptors);
    void extractDescriptor(const Mat &vocabulary, const vector<Mat> target_imgs, Mat &descriptors);
    void saveKmeansResult(const Mat &labels, const vector<Mat> &images, const QString &saveDir, const int cluster_count);

};

#endif // MAINWINDOW_H
