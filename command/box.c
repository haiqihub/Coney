//
//  main.c
//  推箱子
//
//  Created by yq on 2019/8/31.
//  Copyright © 2019 yq. All rights reserved.
//

#include <stdio.h>

int main() {
    printf("Welcome to Push Box Game!\n");
    printf("1--actor\n");
    printf("2--box\n");
    printf("-2--goal\n");
    printf("9--wall\n");
    
    char image[4][4]={
        0,1,0,0,
        0,9,2,0,
        0,0,0,0,
        0,0,0,0};
    
    int flag=0;
    int goalx=3;
    int goaly=1;
    int chestx=1;
    int chesty=2;
    int perx=0;
    int pery=1;
    int operation=0;
    
    
    int i;  //行循环变量
    int j;  //列循环变量
    for (i=0; i<4; ++i)
        {
        for (j=0; j<4; ++j)
            {
                if(i==goalx && j==goaly){
                    printf("%d  ", -2);
                 }
                 else{
                    printf("%d  ", image[i][j]);
                 }
             }
            printf("\n");
         }
    
    printf("Now you can use 1--left 3--right 5--up 2--down to begin game!\n");
    while(flag==0){
        scanf("%d",&operation);
    if(operation==1){
        if(pery==0|| image[perx][pery-1]==9 || (image[perx][pery-1]==2&&pery==1 ||(image[perx][pery-1]==2&&pery>=2 &&image[perx][pery-2]==9))){
            printf("You can not go left!\n");
        }
        else if(image[perx][pery-1]==2&&pery>=2 &&image[perx][pery-2]==0 ){
            chesty=chesty-1;
            
            image[perx][pery]=0;
          pery=pery-1;
            image[perx][pery]=1;
            image[perx][pery-1]=2;
            
        }
        else{
            image[perx][pery]=0;
            pery=pery-1;
            image[perx][pery]=1;
            
        }
    }
    else if(operation==3){
        if(pery==3|| image[perx][pery+1]==9 || (image[perx][pery+1]==2&&pery==2 ||(image[perx][pery+1]==2&&pery<=1 &&image[perx][pery+2]==9))){
                   printf("You can not go right!\n");
               }
        else if(image[perx][pery+1]==2&&pery<=1 &&image[perx][pery+2]==0 ){
                   chesty=chesty+1;
                   
                   image[perx][pery]=0;
                 pery=pery+1;
                   image[perx][pery]=1;
                   image[perx][pery+1]=2;
                  
               }
        else{
                   image[perx][pery]=0;
                   pery=pery+1;
                   image[perx][pery]=1;
                  
               }
    }
    else if(operation==2){
       if(perx==3|| image[perx+1][pery]==9 || (image[perx+1][pery]==2&&perx==2 ||(image[perx+1][pery]==2&&perx<=1 &&image[perx+2][pery]==9))){
                  printf("You can not go down!\n");
              }
       else if(image[perx+1][pery]==2&&perx<=1 &&image[perx+2][pery]==0 ){
                  chestx=chestx+1;
                  
                  image[perx][pery]=0;
                perx=perx+1;
                  image[perx][pery]=1;
                  image[perx+1][pery]=2;
                 
              }
       else{
                  image[perx][pery]=0;
                  perx=perx+1;
                  image[perx][pery]=1;
                  
              }
    }
    else if(operation==5){
       if(perx==0|| image[perx-1][pery]==9 || (image[perx-1][pery]==2&&perx==1 ||(image[perx-1][pery]==2&&perx>=2 &&image[perx-2][pery]==9))){
                  printf("You can not go up!\n");
              }
       else if(image[perx-1][pery]==2&&perx>=2 &&image[perx-2][pery]==0 ){
                  chestx=chestx-1;
                  
                  image[perx][pery]=0;
                perx=perx-1;
                  image[perx][pery]=1;
                  image[perx-1][pery]=2;
                  
              }
       else{
                  image[perx][pery]=0;
                  perx=perx-1;
                  image[perx][pery]=1;
                  
              }
    }
    
    
    else{
            
        }
    

    for (i=0; i<4; ++i)
    {
        for (j=0; j<4; ++j)
        {
            if(i==goalx && j==goaly){
                if(goalx==chestx&&goaly==chesty){
                printf("%d  ", 0);
                    flag=1;
                }
                else {
                    printf("%d  ", -2);
                    
                }
            }
            else{
            printf("%d  ", image[i][j]);
            }
        }
        printf("\n");
    }
    }
    printf("Congratulations on your successful performance!");
    
    return 0;
}
