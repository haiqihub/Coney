//
//  main.c
//  井字棋
//
//  Created by yq on 2019/8/31.
//  Copyright © 2019 yq. All rights reserved.
//

#include <stdio.h>

int main() {
    char image[3][3]={
    0,0,0,
    0,0,0,
    0,0,0};
    printf("Welcome To TicTacToe!\n");
    int i;  //行循环变量
    int j;  //列循环变量
       for (i=0; i<3; ++i)
           {
           for (j=0; j<3; ++j)
               {
                   
                       printf("%d", image[i][j]);
                
                }
               printf("\n");
            }
    int x=0;
    int y=0;
    int flag=0;
   
   
    while(flag==0){
        
    printf("First gamer,please input your (x y)!\n");
    scanf("%d %d",&x,&y);
    image[x][y]=1;
    for (i=0; i<3; ++i)
           {
           for (j=0; j<3; ++j)
               {
                   
                       printf("%d", image[i][j]);
                
                }
               printf("\n");
            }
        if((image[0][0]==1 && image[0][1]==1 && image[0][2]==1)||
           (image[1][0]==1 && image[1][1]==1 && image[1][2]==1)||
           (image[2][0]==1 && image[2][1]==1 && image[2][2]==1)||
           (image[0][0]==1 && image[1][0]==1 && image[2][0]==1)||
           (image[0][1]==1 && image[1][1]==1 && image[2][1]==1)||
           (image[0][2]==1 && image[1][2]==1 && image[2][2]==1)||
           (image[0][0]==1 && image[1][1]==1 && image[2][2]==1)||
           (image[0][2]==1 && image[1][1]==1 && image[2][0]==1)){
            printf("First gamer win!\n");
            flag=1;
            return 0;
        }
    printf("Second gamer,please input your (x y)!\n");
    scanf("%d %d",&x,&y);
    image[x][y]=2;
    for (i=0; i<3; ++i)
           {
           for (j=0; j<3; ++j)
               {
                 
                       printf("%d", image[i][j]);
                   
                }
               printf("\n");
            }
        
        
        if((image[0][0]==2 && image[0][1]==2 && image[0][2]==2)||
           (image[1][0]==2 && image[1][1]==2 && image[1][2]==2)||
           (image[2][0]==2 && image[2][1]==2 && image[2][2]==2)||
           (image[0][0]==2 && image[1][0]==2 && image[2][0]==2)||
           (image[0][1]==2 && image[1][1]==2 && image[2][1]==2)||
           (image[0][2]==2 && image[1][2]==2 && image[2][2]==2)||
           (image[0][0]==2 && image[1][1]==2 && image[2][2]==2)||
           (image[0][2]==2 && image[1][1]==2 && image[2][0]==2)){
            printf("Second gamer win!\n");
            flag=1;
            return 0;
        }
   
    }
    
}
