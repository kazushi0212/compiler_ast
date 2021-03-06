#include<stdio.h>
#include<stdlib.h> 
#include <string.h>
#include "AST.h"

int Decl_num=0; // Decl_AStの時，一度だけ .data 0x10004000を表示するため
int def_n=0; // defineの時に，identの処理を行わない
int ast_n = 0;
int main_num=0;
int loop_num=0;
int loop_rec=0;
int if_num=0;
int else_count=0;
int if_end_num=0;

int t_reg=0; //レジスタ$t　の引数
int a=0;
int b=0;   //算術式の際適切なt_regを指定
int tmp=0;
 

//////////
int LT_a=0;
int LT_b=0;
//////////
Node *build_1_child(NType t,Node *p1){
    Node *p;
    if((p = (Node *)malloc(sizeof(Node))) == NULL){
        yyerror("out of memory");
    }
    p->type=t;
    p->child = p1;
    p->brother=NULL;
    return p;
}

Node *build_child(NType t,Node *p1,Node *p2){
    Node *p;
    if((p = (Node *)malloc(sizeof(Node))) == NULL){
        yyerror("out of memory");
    }
    p->type=t;
    p->child = p1;
    p1->brother = p2;
    p->brother=NULL;
    return p;
}


Node *build_3_child(NType t,Node *p1,Node *p2,Node *p3){
    Node *p;
    if((p = (Node *)malloc(sizeof(Node))) == NULL){
        yyerror("out of memory");
    }
    p->type=t;
    p->child = p1;
    p1->brother = p2;
    p2->brother = p3;
    p->brother=NULL;
    return p;
}


Node *build_4_child(NType t,Node *p1,Node *p2,Node *p3,Node *p4){
    Node *p;
    if((p = (Node *)malloc(sizeof(Node))) == NULL){
        yyerror("out of memory");
    }
    p->type=t;
    p->child = p1;
    p1->brother = p2;
    p2->brother = p3;
    p3->brother = p4;
    p->brother=NULL;
    return p;
}



Node *build_num_node(NType t,int n){
    Node *p;
    if((p = (Node *)malloc(sizeof(Node))) == NULL){
        yyerror("out of memory");
    }
    p->type=t;
    p->value = n;
    p->child = NULL;
    p->brother = NULL;
    return p;
}


Node *build_ident_node(NType t,char *str){
    Node *p;
    if((p = (Node *)malloc(sizeof(Node))) == NULL){
        yyerror("out of memory");
    }
    p->type=t;
    p->varName = (char*)malloc(sizeof(char)*strlen(str));
    strcpy(p->varName,str);
    p->child = NULL;
    p->brother = NULL;
    return p;
}

Node *build_array_node(NType t,char *str,int n){
    Node *p,*p1,*p2;
    p1=build_ident_node(IDENT_AST,str);
    p2=build_num_node(NUM_AST,n);
    p=build_child(t,p1,p2);
    return p;
}

void make_init(FILE *fp){
    char str[]="\
    INITIAL_GP = 0x10008000     # initial value of global pointer \n\
    INITIAL_SP = 0x7ffffffc     # initial value of stack pointer \n\
    # system call service number \n\
    stop_service = 99 \n\
\n\
    .text \n\
init: \n\
    # initialize $gp (global pointer) and $sp (stack pointer) \n\
    la	$gp, INITIAL_GP    # (下の2行に置き換えられる) \n\
#   lui	$gp, 0x1000     # $gp ← 0x10008000 (INITIAL_GP) \n\
#   ori	$gp, $gp, 0x8000 \n\
    la	$sp, INITIAL_SP     # (下の2行に置き換えられる) \n\
#   lui	$sp, 0x7fff     # $sp ← 0x7ffffffc (INITIAL_SP) \n\
#   ori	$sp, $sp, 0xfffc \n\
    jal	main            # jump to `main' \n\
    nop             # (delay slot) \n\
    li	$v0, stop_service   # $v0 ← 99 (stop_service) \n\
    syscall             # stop \n\
    nop \n\
    # not reach here \n\
stop:                   # if syscall return  \n\
    j stop              # infinite loop... \n\
    nop             # (delay slot) \n";
    fprintf(fp,"%s\n",str);
}

void checkNode(Node *p,FILE *fp){
  if(p->child != NULL){
      printTree(p->child,fp);
  }

  if(p->brother != NULL){
      printTree(p->brother,fp);
  } 
}


void regcheck(Node *p,FILE *fp){
    if(a!=0) a=0;
    if(b!=0) b=0;

    printTree(p->child,fp);
            if(a!=0 && b==0){
                tmp=a;
            }
            if(b!=0 && a==0){
                tmp=b;
            }           
            //ast_n=1; //identでlwの処理を行うかどうか
    
            if(p->child->brother != NULL){
                ast_n=1; //identでlwの処理を行うかどうか,項が3以上の算術式の時の変数呼び出しに必要

              if(t_reg>9){  //レジスタの利用規則0~9までの確認用
                    printf("\n The current reg_num %d is bigger than reg_num 9 of the rule!\n",t_reg);
                }

                printTree(p->child->brother,fp);
            } 
            
            if(t_reg>9){   //レジスタの利用規則0~9までの確認用
                printf("\n The current reg_num %d is bigger than 9 !\n",t_reg);
            }

            if(a!=0 && b==0){
                b=a;
                a=tmp;
            }
            if(a==0 && b!=0){
                a=tmp;
            }

            if(a>b){
                tmp=a;
                a=b;
                b=tmp;
            }
}
void printTree(Node *p,FILE *fp){
  if(p != NULL){
        switch(p->type){
        case Pro_AST:
            make_init(fp);
            printf("PROGRAM\n");
            checkNode(p,fp);
            fprintf(fp,"$EXIT: \n \tjr   $ra \t;in Pro_AST \n \tnop");
            break;
        case Decl_AST:
            printf("DECLARATION\n");
            if(Decl_num==0){
                fprintf(fp,"\t.data   0x10004000 \t;in Decl_AST \n");
                Decl_num++;
            }
            checkNode(p,fp);
            break;
        case Decl_stmt_AST:
            printf("Decl_statement\n");
            checkNode(p,fp);
            break;
        case Stmts_AST:
            printf("statements\n");
            if(main_num==0){
            fprintf(fp,"\t.text 	0x00001000 \t;in Stmts_AST \n");
            fprintf(fp,"main: \t;in Stmts_AST \n");
            main_num++;
            }
            //checkNode(p,fp);

            printTree(p->child,fp);  

            break;
        case Stmt_AST:
            printf("statement\n");
            checkNode(p,fp);
            break;
            
////////NUM,IDENT/////
        case IDENT_AST:
            printf("%s\t",p->varName);
            if(def_n == 0){
                fprintf(fp,"\tli   $t%d,%s  \t;in IDENT_AST \n",t_reg,p->varName);
                if(ast_n != 0){
                    fprintf(fp,"\tlw   $t%d,0($t%d) \t;in IDENT_AST \n",t_reg+1,t_reg);
                    fprintf(fp,"\tnop \t;in IDENT_AST \n");
                    t_reg++;                   
                    a=t_reg;                    
                }
                t_reg++;
            }
            //checkNode(p,fp);
            break;
        case NUM_AST:
            printf("%d\t",p->value);
            fprintf(fp,"\tli   $t%d,%d \t;in NUM_AST \n",t_reg,p->value);
            b=t_reg;
            //printf("!!%d!!\n",b);
            t_reg++;
            //checkNode(p,fp);
            break;

        case Expr_AST:
            printf("expression\n");
            printTree(p->child,fp);
            //checkNode(p,fp);            
            break;
        case Term_AST:
            printf("term\n");
            printTree(p->child,fp);
            //checkNode(p,fp);
            break;
        case Factor_AST:
            printf("factor\n");
            printTree(p->child,fp);
            //checkNode(p,fp);
            break;
            
//////////DEFINE,ARRAY
        case DEFINE_AST:
            printf("DEFINE\n");
            fprintf(fp,"%s:\t.word  0x0000 \t;in DEFINE_AST \n",p->child->varName);
            def_n=1;
            checkNode(p,fp);
            def_n=0;
            break;
        case ARRAY_AST:
            printf("ARRAY\n");
            checkNode(p,fp);
            break;
	    
/////WHILE,IF
        case WHILE_AST:
            printf("WHILE\n");

            fprintf(fp,"$L%d_%d:\n",loop_rec,loop_num);
            checkNode(p,fp);
            if(p->child->type != EQ_AST){
                fprintf(fp,"\tbeq   $t%d,$zero,$L%d_%d \t;in WHILE_AST \n",t_reg,loop_rec,loop_num+1);
            }
            if(p->child->type == EQ_AST){
                fprintf(fp,"\tbne   $t%d,$t%d,$L%d_%d \t;in WHILE_AST \n",a,b,loop_rec,loop_num+1);
            }
            t_reg=0;
            loop_rec++;
            printTree(p->child->brother,fp);
            loop_rec--;
            fprintf(fp,"\tj $L%d_%d \t;in WHILE_AST \n",loop_rec,loop_num);
            fprintf(fp,"\tnop \t;in WHILE_AST \n");
            fprintf(fp,"$L%d_%d: \t;in WHILE_AST \n",loop_rec,loop_num+1);
            if(loop_rec==0) loop_num=loop_num+2;
            a=0;
            b=0;
            break;
        case IF_AST:
            printf("IF\n");
            fprintf(fp,";!!!!!!!!!!IF!!!!!!!!!!!\n");
            checkNode(p,fp);
      
            if(p->child->type != EQ_AST){
                fprintf(fp,"\tbeq   $t%d,$zero,$D%d \t;in IF_AST \n",t_reg,if_num);
                else_count++;
            }
            if(p->child->type == EQ_AST){
                fprintf(fp,"\tbne   $t%d,$t%d,$D%d \t;in IF_AST \n",a,b,if_num);
                else_count++;
            }
            t_reg=0;
            printTree(p->child->brother,fp);
            if(p->child->brother->brother!=NULL){   //else if ,elseが存在する時
            printTree(p->child->brother->brother,fp); 
            }
            fprintf(fp,"\tnop \t;in IF_AST\n");
            fprintf(fp,"$D0_%d: \t;in IF_AST\n",if_end_num);
            
            fprintf(fp,"$D%d:\n",if_num); //if文の終点のラベル
            a=0;
            b=0;
            if_num++;
            if_end_num++;
            break;

        case ELSEIF_AST:
            fprintf(fp,";!!!!!!!!!!ELSEIF!!!!!!!!!!!\n");
            printf("ELSEIF\n");
            fprintf(fp,"\tj   $D0_%d \t;ELSEIF_AST \n ",if_end_num);
            fprintf(fp,"\tnop \n");
            fprintf(fp,"$D%d: \t;in ELSEIF_AST \n",if_num);
          
            printTree(p->child,fp);

            if(p->child->type != EQ_AST){
                if(p->brother !=NULL || else_count!=0){
                    fprintf(fp,"\tbeq   $t%d,$zero,$D%d \t;in ELSEIF_AST \n ",t_reg,if_num+1);
                    else_count++;
                } else {
                    fprintf(fp,"\tbeq   $t%d,$zero,$D0 \t;in ELSEIF_AST \n ",t_reg);
                }
            }
            if(p->child->type == EQ_AST){
                if(p->brother !=NULL || else_count!=0){
                fprintf(fp,"\tbne   $t%d,$t%d,$D%d \t;in ELSEIF_AST\n ",a,b,if_num+1);
                else_count++;
                } else{
                    fprintf(fp,"\tbne   $t%d,$t%d,$D0_%d \t;in ELSEIF_AST\n ",a,b,if_end_num);
                }
            }    
            t_reg=0;
            if_num++;
            printTree(p->child->brother,fp);

            if(p->child->brother->brother!=NULL){
                printTree(p->child->brother->brother,fp);
            }

            if(p->brother!=NULL){
                printTree(p->brother,fp);
            }
            t_reg=0;
            a=0;
            b=0;
            break;

        case ELSE_AST:
            printf("ELSE\n");
            fprintf(fp,";!!!!!!!!!!ELSE!!!!!!!!!!!\n");
            fprintf(fp,"\tj   $D0_%d \t;in ELSE_AST \n",if_end_num);
            fprintf(fp,"\tnop \t;in ELSE_AST \n");
            fprintf(fp,"$D%d: \t;in ELSE_AST \n",if_num);
            checkNode(p,fp);
            if_num++;
            break;            


//////算術式
        case ASSIGN_AST:
            printf(" = \n");
            //checkNode(p,fp);
            printTree(p->child,fp);
            printTree(p->child->brother,fp);
            fprintf(fp,"\tsw   $t%d,0($t0)\n",t_reg-1);
            t_reg=0;
            a=0;
            b=0;
            break;

        case ADD_AST:
            ast_n=1; //identでlwの処理を行うかどうか
            printf(" + \n");

            regcheck(p,fp);
            fprintf(fp,"\tadd   $t%d,$t%d,$t%d\n",t_reg,a,b);

            ast_n=0; //初期化
            t_reg++;
            a=t_reg-1;
            b=t_reg-1;
            tmp=0;
            break;
           
        case SUB_AST:
            ast_n=1; //identでlwの処理を行うかどうか
            printf(" - \n");

            regcheck(p,fp);
            fprintf(fp,"\tsub   $t%d,$t%d,$t%d\n",t_reg,a,b);

            ast_n=0; //初期化
            t_reg++;
            a=t_reg-1;
            b=t_reg-1;
            tmp=0;
            break;
        case MUL_AST:
            ast_n=1; //identでlwの処理を行うかどうか
            printf(" * \n");
            regcheck(p,fp);

            fprintf(fp,"\tmult   $t%d,$t%d\n",a,b);
            fprintf(fp,"\tmflo   $t%d\n",t_reg);

            ast_n=0; //初期化
            t_reg++;
            a=t_reg-1;
            b=t_reg-1;
            tmp=0;
            break;
        case DIV_AST:
            ast_n=1; //identでlwの処理を行うかどうか
            printf(" / \n");        

            regcheck(p,fp);
            printf("!!!check3:a,%d,,b,%d!!!\n",a,b);
            fprintf(fp,"\tdiv   $t%d,$t%d\n",a,b);
            fprintf(fp,"\tmflo   $t%d\n",t_reg);

            ast_n=0; //初期化
            t_reg++;
            a=t_reg-1;
            b=t_reg-1;
            tmp=0;
            break;

        case MOD_AST:
            ast_n=1; //identでlwの処理を行うかどうか
            printf(" / \n");        
            regcheck(p,fp);
   
            fprintf(fp,"\tdiv   $t%d,$t%d\n",a,b);
            fprintf(fp,"\tmfhi   $t%d\n",t_reg);

            ast_n=0; //初期化
            t_reg++;
            a=t_reg-1;
            b=t_reg-1;
            tmp=0;
            break;


//比較
        case EQ_AST: //右辺が算術式の時エラー
            ast_n=1;   //identでlwの処理がいる場合とで場合分け
            printf(" == \n");
            regcheck(p,fp);
            //fprintf(fp,"\tbne   $t%d,$t%d,$L%d\n",a,b,loop_num+1);
            ast_n=0;     //初期化
/*
            a=0;
            b=0;
            tmp=0;
*/
            break;

        case LT_AST:
            printf(" < \n");
            ast_n=1;
            printTree(p->child,fp);
            LT_a=a; 
            printTree(p->child->brother,fp);
            LT_b=t_reg-1;
            fprintf(fp,"\tslt   $t%d,$t%d,$t%d\n",t_reg,LT_a,LT_b);
            a=0;
            b=0;
            LT_a=0;
            LT_b=0;
            ast_n=0;
            break;
/*
            ast_n=1;   //identでlwの処理がいる場合とで場合分け
            printf(" < \n");
            regcheck(p,fp);
            fprintf(fp,"\tslt   $t%d,$t%d,$t%d\n",t_reg,a,b);
            a=0;
            b=0;
            ast_n=0;     //初期化
            tmp=0;
            break;
*/
        case GT_AST:
            printf(" < \n");
            ast_n=1;
            printTree(p->child,fp);
            LT_a=a; //課題4のためだけ
            printTree(p->child->brother,fp);

            LT_b=t_reg-1;
            fprintf(fp,"\tslt   $t%d,$t%d,$t%d\n",t_reg,LT_b,LT_a);
            a=0;
            b=0;
            LT_a=0;
            LT_b=0;
            ast_n=0;
            break;
/*
            ast_n=1;   //identでlwの処理がいる場合とで場合分け
            printf(" > \n");
            regcheck(p,fp);
            fprintf(fp,"\tslt   $t%d,$t%d,$t%d\n",t_reg,b,a);
            a=0;
            b=0;
            ast_n=0;     // 初期化
            tmp=0;
            break;
*/

/*
case LET_AST:
先にイコールの判定をしてOKならwhileの処理にジャンプ，つぎに＜の判定をしてOKならwhileの処理にジャンプ
whileにLET_ASTようの処理を追加し，判定前に$v0に1を入れておき，判定が正解なら1がはいり，不正解なら0がはいるので，whileでその判定を行う．

*/

        default :
            fprintf(stderr,"print error\n");
        }
    }
}







