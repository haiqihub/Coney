//
//  main.c
//  jjj
//
//  Created by yq on 2019/8/31.
//  Copyright © 2019 yq. All rights reserved.
//

#include <stdio.h>

int main() {
    
    int a = 0;
    int b = 0;
    int operation;
    int result = 0;
    printf("Corresponding Number Represents Operation\n");
    printf("1--add\n");
    printf("2--minus\n");
    printf("3--multiply\n");
    printf("4--divide\n");
    printf("Please enter corresponding number\n");
    scanf("%d",&operation);
    printf("Please enter first Operate number\n");
    scanf("%d",&a);
    printf("Please enter second Operate number\n");
    scanf("%d",&b);
    if(operation==1){
        result=a+b;
        printf("%d add %d is %d\n",a,b,result);
    }
    else if(operation==2){
        result=a-b;
        printf("%d minus %d is %d\n",a,b,result);
    }
    else if(operation==3){
        result=a*b;
        printf("%d multiply %d is %d\n",a,b,result);
    }
    else if(operation==4){
        result=a/b;
        printf("%d divide %d is %d\n",a,b,result);
    }
   
    return 0;
}


