// usage:   ./output infile outfile Q
//          Q = 1 to 100

#include "utils.hpp"
#include "huffmanDecoding.hpp"

Mat decompressImg(char* filename) {
    char fname[] = "temp2.txt";
    // FILE* fpp = fopen(fname, "w");
    // fclose(fpp);
    huffmanDecode(filename, fname);
    FILE* fp = fopen("temp.txt", "r");
    int rows, cols, Q;
    fscanf(fp, "%d%d%d", &rows, &cols, &Q);
    createDctMatrix();
    Mat compressed(rows, cols, CV_8UC3);
    
    uint8 qmaty[N][N];
    uint8 qmatc[N][N];
    getQuantizationMatrix(qmaty, Q, LUM);
    getQuantizationMatrix(qmatc, Q, CHROM);
    
    for (int i = 0; i < N*ceil(rows/(double)N); i+=N)
    {
        for (int j = 0, j2 = 0; j < N*ceil(cols/(double)N); j+=N, j2 += 3*N)
        {
            for (int l = 0; l < 3; l++)
            {
                // Mat block = getBlock(padded, i, j2, l)-128;
                // Mat fdct = getDct(block);
                // Mat quantized = (l == 0) ? divideMat(fdct, qmaty) : divideMat(fdct, qmatc);
                // int q_block[N][N], zz_arr[N*N];
                // convertMatToArr(quantized, q_block);
                // zigzag(q_block, zz_arr);
                // run_length_encode(zz_arr, fp);
                int q_block[N][N], zz_arr[N*N];
                run_length_decode(zz_arr, fp);
                zigzag_to_block(q_block, zz_arr);
                Mat block = convertArrToMat(q_block);
                Mat dequantized = (l == 0) ? multiplyMat(block, qmaty) : multiplyMat(block, qmatc);
                Mat idct = getIDct(dequantized);

                for (int m = 0; m < N; m++)
                {
                    if(i+m >= rows) break;
                    cptr(ip, m, idct, double);
                    ptr(op, i+m, compressed, uint8);
                    for (int n1 = 0, n2 = 0; n1 < N; n1++, n2+=3)
                    {
                        if(j+n1 >= cols) break;
                        op[j2+n2+l] = roundPixel127(ip[n1]) + 128;
                    }
                    
                }
                
            }
        }
    }
    fclose(fp);
    return convertToRGB(compressed);
}

int main(int argc, char *argv[])
{

    Mat c = decompressImg(argv[1]);
    namedWindow("Decompressed Image", WINDOW_AUTOSIZE);
    imshow("Decompressed Image", c);
    waitKey(0);
    return 0;
}
