//Proj9.1 Morphology Ops.
//Algm by Dr. Tsiyun Phillips
//Implemented by Shawn Yang, 5/4/2018
#include <iostream>
#include <fstream>
using namespace std;

class MorphologyOps{
public:
    ifstream infile1_img;
    ifstream infile2_structElem;
    ofstream outfile1_dilation;
    ofstream outfile2_erosion;
    ofstream outfile3_closing;
    ofstream outfile4_opening;
    
    int numRowsImg;
    int numColsImg;
    int minValImg;
    int maxValImg;
    int numRowsStructElem;
    int numColsStructElem;
    int minValStructElem;
    int maxValStructElem;
    int rowOrigin;
    int colOrigin;
    int rowFrameSize;
    int colFrameSize;
    
    int** imgAry;
    int** morphAry;
    int** structElemAry;
    MorphologyOps(const char** argv){
        //step0. initializing I/O
        try {
            infile1_img.open(argv[1]);
            infile2_structElem.open(argv[2]);
            outfile1_dilation.open(argv[3]);
            outfile2_erosion.open(argv[4]);
            outfile3_closing.open(argv[5]);
            outfile4_opening.open(argv[6]);
        } catch (exception *e) {
            cout<<"error opening files."<<endl;
            exit(1);
        }
        infile1_img>>numRowsImg>>numColsImg>>minValImg>>maxValImg;
        infile2_structElem>>numRowsStructElem>>numColsStructElem>>minValStructElem>>
        maxValStructElem>>rowOrigin>>colOrigin;
        //step1 compute frameSize
        computeFrameSize();
        //step2,3
        imgAry= newImageAryObjectMaker(numRowsImg+rowFrameSize, numColsImg+colFrameSize);
        morphAry= newImageAryObjectMaker(numRowsImg+rowFrameSize, numColsImg+colFrameSize);
        structElemAry=new int*[numRowsStructElem];
        for (int i=0; i<numRowsStructElem;i++){
            structElemAry[i]=new int [numColsStructElem];
        }
        loadImg();
        zeroFrameImg();
        cout<<"loaded imgAry:"<<endl;
        prettyPrint(imgAry, numRowsImg+rowFrameSize, numColsImg+colFrameSize);
        loadStructElem();
        cout<<"loaded structElemAry:"<<endl;
        prettyPrint(structElemAry, numRowsStructElem, numColsStructElem);
    }
    
    void run(){
        //step4
        cout<<"Starting Dialation..."<<endl;
        initMorphAry();
        dilation(rowOrigin, colOrigin, imgAry);
        prettyPrint(morphAry, numRowsImg+rowFrameSize, numColsImg+colFrameSize);
        outputResult(&outfile1_dilation);
        //step5
        cout<<"Starting Erosion..."<<endl;
        initMorphAry();
        erosion(rowOrigin, colOrigin, imgAry);
        prettyPrint(morphAry, numRowsImg+rowFrameSize, numColsImg+colFrameSize);
        outputResult(&outfile2_erosion);
        //step6
        cout<<"Starting Closing..."<<endl;
        initMorphAry();
        closing(rowOrigin, colOrigin);
        prettyPrint(morphAry, numRowsImg+rowFrameSize, numColsImg+colFrameSize);
        outputResult(&outfile3_closing);
        //step7
        cout<<"Starting Opening..."<<endl;
        initMorphAry();
        opening(rowOrigin, colOrigin);
        prettyPrint(morphAry, numRowsImg+rowFrameSize, numColsImg+colFrameSize);
        outputResult(&outfile4_opening);
        //step8
        infile1_img.close();
        infile2_structElem.close();
        outfile1_dilation.close();
        outfile2_erosion.close();
        outfile3_closing.close();
        outfile4_opening.close();
    }
    
    void computeFrameSize(){
        rowFrameSize=numRowsStructElem;
        colFrameSize=numColsStructElem;
    }
    void loadImg(){
        int offsetRow=rowFrameSize/2;
        int offsetCol=colFrameSize/2;
        for (int i=offsetRow; i<numRowsImg+offsetRow; i++){
            for (int j=offsetCol; j<numColsImg+offsetCol;j++)
                infile1_img>>imgAry[i][j];
        }
    }
    void loadStructElem(){
        for (int i=0; i<numRowsStructElem; i++){
            for (int j=0; j<numColsStructElem; j++)
                infile2_structElem>>structElemAry[i][j];
        }
    }
    void zeroFrameImg(){
        //todo
    }
    void initMorphAry(){
        for (int i=0; i< numRowsImg+rowFrameSize;i++){
            for (int j=0; j< numColsImg+colFrameSize;j++){
                morphAry[i][j]=0;
            }
        }
    }
    void dilation(int offset_i, int offset_j, int** sourceImgAry){
        for (int i=offset_i; i< numRowsImg+offset_i; i++){
            for (int j= offset_j; j< numColsImg+offset_j; j++){
                //for each point in imgAry, process its neighborhood with the StructElemAry.
                if (sourceImgAry[i][j]==1)
                    dialate_a_point(i, j, sourceImgAry);
            }
        }
    }
    void dialate_a_point(int i, int j, int** sourceImgAry){
        if (sourceImgAry[i][j]==structElemAry[rowOrigin][colOrigin]){
            for (int r=0; r<numRowsStructElem; r++){
                for (int c=0; c<numColsStructElem;c++){
                    if (structElemAry[r][c]==1)
                        morphAry[i-rowFrameSize/2+r][j-colFrameSize/2+c]=1;
                }
            }
        }
    }
    void erosion(int offset_i, int offset_j, int**sourceImgAry){
        for (int i=offset_i; i<numRowsImg+offset_i; i++){
            for (int j=offset_j; j<numColsImg+offset_j; j++){
                if (sourceImgAry[i][j]==1)
                    erode_a_point(i, j, sourceImgAry);
            }
        }
    }
    void erode_a_point(int i,int j, int** sourceImgAry){
        bool isSatisfied=true;
        for (int r=0;r < numRowsStructElem; r++){
            for (int c=0; c<numColsStructElem;c++){
                if (structElemAry[r][c]!=0 && sourceImgAry[i-rowFrameSize/2+r][j-colFrameSize/2+c]!=structElemAry[r][c])
                    isSatisfied=false;
            }
        }
        if (isSatisfied)
            morphAry[i][j]=1;
    }
    void closing (int i, int j){
        int** tempAry; //as an intermediary storage for closing and opening.
        dilation(i, j, imgAry);
        //re-allocate and initialize morphAry
        tempAry=morphAry;
        morphAry= newImageAryObjectMaker(numRowsImg+rowFrameSize, numColsImg+colFrameSize);
        erosion(i, j, tempAry);
        //tempAry is left as is, no need to destruct for its size in this project.
    }
    void opening (int i, int j){
        int** tempAry; //as an intermediary storage for closing and opening.
        erosion(i, j, imgAry);
        //re-allocate and initialize morphAry
        tempAry=morphAry;
        morphAry= newImageAryObjectMaker(numRowsImg+rowFrameSize, numColsImg+colFrameSize);
        dilation(i, j, tempAry);
        //tempAry is left as is, no need to destruct for its size in this project.
    }
    
    int** newImageAryObjectMaker(int numRows, int numCol){
        int** newImageAry = new int*[numRows];
        for (int i=0; i<numRows; i++){
            newImageAry[i]=new int[numCol];
            for (int j=0; j<numCol; j++)
                newImageAry[i][j]=0;
        }
        return newImageAry;
    }
    
    void prettyPrint(int** imageAry, int numRows, int numCols){
        for (int i=0; i<numRows; i++){
            for (int j=0; j<numCols; j++){
                if (imageAry[i][j]>0)
                    cout<< imageAry[i][j]<<" ";
                else
                    cout<< "  ";
            }
            cout<< endl;
        }
    }
    void outputResult(ofstream *outfile){
        int offsetRow=rowFrameSize/2;
        int offsetCol=colFrameSize/2;
        *outfile<<numRowsImg<<" "<<numColsImg<<" "<<minValImg<<" "<<maxValImg<<endl;
        for (int i=offsetRow; i<numRowsImg+offsetRow;i++){
            for (int j=offsetCol; j<numColsImg+offsetCol; j++){
                *outfile<<morphAry[i][j]<<" ";
            }
            *outfile<<endl;
        }
    }
};

int main(int argc, const char * argv[]) {
    if (argc<7){
        cout<<"Incorrect arguments, supply 2 input and 4 output files and try again."<<endl;
        return 1;
    }
    MorphologyOps *morphOps=new MorphologyOps(argv);
    morphOps->run();
    return 0;
}
