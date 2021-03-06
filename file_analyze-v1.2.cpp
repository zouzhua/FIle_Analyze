/* 乱码解决办法
 * 需要设置编码为GBK,否则中文输出乱码
 * 设置GBK编码后,输出正常,但调试时查看变量是还是乱码,不过不影响,程序正确运行
 * is_find,is_find_child未处理
 * */
#include <iostream>
#include <io.h>
#include <time.h>
#include <cstring>
using namespace std;

typedef struct File_Node{
    struct File_Node *front;
    struct File_Node *next;
    string name;
    long long size;
}File_Node,*File;

//存储文件夹大小的树
typedef struct Node {
    struct Node *child;   //第一个子节点
    struct Node *brother; //第二、三、四...节点
    struct File_Node *file;
    string name;
    long long size;
} Node, *Tree;

//is_find:是否找到该路径   is_find_child:是否找到该路径下的子目录
bool is_find = false, is_find_child = false;

//遍历文件夹(DFS)
void GetFiles(string path, Tree file) {
    long hFile = 0;              //文件句柄
    struct _finddata_t fileinfo; //文件信息结构体
    string p;                    //字符串，存放路径
    if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1) {
        Tree last;
        do {
            if ((fileinfo.attrib & _A_SUBDIR)) { //如果是目录
                //(.表示当前目录 ..表示当前目录的父目录)忽略这两个
                if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0) {
                    Node *newNode = new Node;                                          //新建节点
                    newNode->size = 0;                                                 //初始化节点的大小
                    newNode->name = p.assign(path).append("\\").append(fileinfo.name); //初始化节点的路径名
                    newNode->child = newNode->brother = NULL;                          //初始化此节点的第一个孩子节点和此节点的兄弟节点
                    if (file->child == NULL) {                                         //当父节点的第一个孩子节点为空时，表示这个节点是父节点的第一个孩子节点
                        file->child = newNode;                                         //此节点为父节点的第一个孩子节点
                        last = newNode;                                                //此节点存为上一次处理的节点
                    } else {                                                           //父节点的第一个孩子节点不为空，即此节点和上一个处理的节点为兄弟节点，此节点存为上一个处理的节点的兄弟节点
                        last->brother = newNode;                                       //把此节点存为上一个处理的节点的兄弟节点
                        last = newNode;                                                //把此节点存为上一个处理的节点
                    }
                    GetFiles(p.assign(path).append("\\").append(fileinfo.name), newNode); //遍历下一层文件夹和文件
                    file->size += newNode->size;                                          //加上path子文件夹(即此文件夹)的大小
                }
            } else {                         //如果是文件
                file->size += fileinfo.size; //加上path文件夹下文件的大小

                File files=new File_Node;
                files->name=fileinfo.name;
                files->size=fileinfo.size;

                files->front=file->file;
                file->file=files;
            }
        } while (_findnext(hFile, &fileinfo) == 0);
        _findclose(hFile);
    }
}

//查找输入路径下子目录大小
Tree output_dir_size(Tree file, string path) {
    string::size_type idx;
    idx = path.find(file->name);      //比较当前路径(file->name)和输入路径(path)
    if (idx == string::npos) {        //输入路径不包含当前路径
        while (idx == string::npos) { //一直查找到输入路径包含当前路径
            file = file->brother;     //查找同层文件夹
            if (file == NULL)         //同层文件夹查找完毕,但未查找到,退出查找
                return NULL;
            idx = path.find(file->name);
        }
    }
    if (file->name == path) { //如果找到输入的那个路径
        is_find = true;
        return file;
    }
    return output_dir_size(file->child, path);
}

void Display(Tree tree,string path){
	Tree tree_temp = tree->child;
    while (tree_temp) {
        cout << "[文件夹]" << tree_temp->name << " " << tree_temp->size << endl;
        tree_temp = tree_temp->brother;
    }
    while (tree->file) {
        cout << tree->file->name << " " << tree->file->size << endl;
        tree->file = tree->file->front;
    }
}

int main() {
    string file_path = "D:";

    Tree file = new Node;
    file->child = file->brother = NULL;
    file->name = file_path;
    file->size = 0;

    int start_time=clock();
    GetFiles(file_path, file);
    int end_time=clock();
    cout << "完成搜索 " <<end_time-start_time<<"ms"<< endl;

    /*测试 输入:一个路径 输出:路径内文件夹的大小*/
    string path_input;
    while (cin >> path_input && path_input != "exit") {

        Tree temp_node = file;
        temp_node=output_dir_size(temp_node, path_input);
        if(temp_node!=NULL)
            Display(temp_node,path_input);
        else
            cout<<"未找到"<<endl;
    }
    return 0;
}
