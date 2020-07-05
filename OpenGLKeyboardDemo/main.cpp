

#include "GLShaderManager.h"
#include "GLTools.h"
#include <GLUT/GLUT.h>

//定义一个着色器
GLShaderManager shaderManager;

//批次容器，GLTools的一个简单容器类。
GLBatch triangleBatch;

/**
 正方形的边长，
 OpenGL默认坐标原点是屏幕的正中心，屏幕的xy范围是（-1，-1）到（1，1）
 0.1可以理解为屏幕正值总长度的的10%，（正值+负值）的总长度的5%
 由于代码中设置的glut初始化窗口是500x500，所以用这种方式设置边长是正方形，如果修改成500x600，就不是正方形了
 */
GLfloat blockSize = 0.1f;

//正方形4个点 0~2：左下点  3~5：右下点  6~8：右上点 9~11：左上点
GLfloat vVerts[] = {
    -blockSize, -blockSize, 0.0f,
    blockSize, -blockSize, 0.0f,
    blockSize, blockSize, 0.0f,
    -blockSize, blockSize, 0.0f,
};

GLfloat xPos = 0.0f;
GLfloat yPos = 0.0f;

GLfloat lRot = 0.0f;

//修改窗口大小回调
void changeSize(int w, int h) {
    //设置视口
    glViewport(0, 0, w, h);
}

//渲染显示回调
void renderScene(void) {
    /*
     clear缓冲区
     缓冲区是一块存在图像信息的储存空间，红色、绿色、蓝色和alpha分量通常一起分量通常一起作为颜色缓存区或像素缓存区引用。
     OpenGL 中不止一种缓冲区（颜色缓存区、深度缓存区和模板缓存区）
     清除缓存区对数值进行预置
     参数：指定将要清除的缓存的
     GL_COLOR_BUFFER_BIT :指示当前激活的用来进行颜色写入缓冲区
     GL_DEPTH_BUFFER_BIT :指示深度缓存区
     GL_STENCIL_BUFFER_BIT:指示模板缓冲区
    */
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
    GLfloat vRed[] = {1.0f, 0.0f, 0.0f, 0.0f};
    M3DMatrix44f mFinalTransform, mTransfomrmMatrix, mRotationMartix;
    
    //平移
    m3dTranslationMatrix44(mTransfomrmMatrix, xPos, yPos, 0.0f);
    
    //旋转
    m3dRotationMatrix44(mRotationMartix, m3dDegToRad(lRot), 0.0f, 0.0f, 1.0f);
    
    //旋转和移动的矩阵结果 合并到mFinalTransform（矩阵相乘）
    m3dMatrixMultiply44(mFinalTransform, mTransfomrmMatrix, mRotationMartix);
    
    //shaderManager调用固定着色器方法，参数指定着色器类型
    
//    shaderManager.UseStockShader(GLT_SHADER_IDENTITY, vRed);
    shaderManager.UseStockShader(GLT_SHADER_FLAT, mFinalTransform, vRed);
    
    //提交着色器
    triangleBatch.Draw();
    
    //在开始的设置openGL 窗口的时候，我们指定要一个双缓冲区的渲染环境。这就意味着将在后台缓冲区进行渲染，渲染结束后交换给前台。这种方式可以防止观察者看到可能伴随着动画帧与动画帧之间的闪烁的渲染过程。缓冲区交换平台将以平台特定的方式进行。
    //将后台缓冲区进行渲染，然后结束后交换给前台
    glutSwapBuffers();
}

void setupRC() {
    //设置清屏颜色（背景颜色）
    glClearColor(0.98, 0.4, 0.7, 1);
    
    //初始化着色器管理类
    shaderManager.InitializeStockShaders();
    
    //开始批处理
    triangleBatch.Begin(GL_TRIANGLE_FAN, 4);
    //拷贝顶点数据
    triangleBatch.CopyVertexData3f(vVerts);
    //结束批处理
    triangleBatch.End();
}

void SpeciaKeys(int key, int x, int y) {
    GLfloat stepSize = 0.025f;
    
    /**
     键盘响应相关处理，对应 上下左右
     GLUT_KEY_UP
     GLUT_KEY_DOWN
     GLUT_KEY_LEFT
     GLUT_KEY_RIGHT
     */
    if (key == GLUT_KEY_UP) {
        yPos += stepSize;
    }
    
    if (key == GLUT_KEY_DOWN) {
        yPos -= stepSize;
    }
    
    if (key == GLUT_KEY_LEFT) {
        xPos -= stepSize;
        lRot += 5;
    }
    
    if (key == GLUT_KEY_RIGHT) {
        xPos += stepSize;
        lRot -= 5;
    }
    
    //边界处理
    if (xPos < -1.0 + blockSize) {
        xPos = -1.0 + blockSize;
    }
    
    if (xPos > 1.0 - blockSize) {
        xPos = 1.0 - blockSize;
    }
    
    if (yPos > 1.0 - blockSize) {
        yPos = 1.0 - blockSize;
    }
    
    if (yPos < -1.0 + blockSize) {
        yPos = -1.0 + blockSize;
    }
    
    glutPostRedisplay();
}

int main(int argc,char *argv[]) {
    
    /**
     设置工作路径。是GLTools中的函数。在Mac中，将工作目录修改为app中的Resource目录。而windows中不需要，因为windows中默认的把工作目录与程序的可执行程序的目录设置为相同。
     */
    
    gltSetWorkingDirectory(argv[0]);
    
    //初始化glut库，将app的入口函数main中的参数传递给glut
    glutInit(&argc, argv);
    
    /**
     设置glut显示模式
     GLUT_DOUBLE: 双缓存窗口，是指绘图命令实际上是离屏缓存区执行的，然后迅速转换成窗口视图，这种方式经常用来生成动画效果
     GLUT_RGBA：表明欲建立RGBA模式的窗口。
     GLUT_DEPTH：标志将一个深度缓存区分配为显示的一部分，因此我们能够执行深度测试
     GLUT_STENCIL：确保我们也会有一个可用的模板缓存区。
     */
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
    
    //glut初始化窗口 500x500
    glutInitWindowSize(500, 500);
    //glut创建窗口，并设置窗口title
    glutCreateWindow("Triangle");
    
    /**
     GLUT内部运行一个本地消息循环，拦截适当的消息。然后调用注册的回调函数。
     */
    //注册重塑函数，为窗口改变size的时候进行回调处理
    glutReshapeFunc(changeSize);
    //注册显示函数，渲染回调
    glutDisplayFunc(renderScene);
    //注册特殊函数，触发键盘操作
    glutSpecialFunc(SpeciaKeys);
    
    /**
     初始化一个GLEW库，确保OpenGL API是可用有效的。
     在试图做任何渲染之前，要检查确定驱动程序的初始化过程中没有任何问题
     */
    GLenum status = glewInit();
    if (GLEW_OK != status) {
        printf("GLEW Error:%s\n", glewGetErrorString(status));
        return 1;
    }
    
    //初始化设置我们的渲染环境
    setupRC();
    //glut启动
    glutMainLoop();
    
    return 0;
}
