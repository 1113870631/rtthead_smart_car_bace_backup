#include "thead_creat.h"

#include <rtdevice.h>
#include <thead_creat.h>
#include <motrol.h>
#include <motrol_dir.h>
#define LOG_TAG "thead_creat.c"
#define LOG_LVL LOG_LVL_DBG
#include <ulog.h>
/*命令缓冲区
 * [0] [0/1/-1] [speed][保留]
 * [0]:代表是速度控制命令
 * [0/1/-1] 0 停止  1 前进 -1 后退
 * 完整命令实例：  0 1 0 0
 *
 * [1] [pwm1] [pwm1] [speed]
 * [1]:代表是方向控制命令
 * [pwm1] 占空比的第一个数
 * [pwm2] 占空比的第二个数
 * [speed] 代表速度 1 代表10%的速度 2 20% 0 100%
 *完整命令实例：1 50 1
 * */
char command_move_pool[4];
char command_dir_pool[4];
int num=0;
int encoder_num=0;
rt_sem_t rx_sem,move_sem,dir_sem;
/*线程1 入口 接收上层命令 传输到对应的命令缓冲区*/
static void uart_re_entry(void *parameter){

    char buffer[4];
    #define SAMPLE_UART_NAME "uart2" /* 串 口 设 备 名 称 */
    static rt_device_t serial; /* 串 口 设 备 句 柄 */
    rt_err_t ret,result;

/*接收回调函数 接收4个字节发送信号*/
    rt_err_t uart_input(rt_device_t dev, rt_size_t size) {
        rt_kprintf("size is %d\n",size);
        if(size==4)
        {
            rt_sem_release(rx_sem);
        };
        //rt_kprintf("%d\n",size);
        return RT_EOK;
    }
    /*找到uart2*/
    serial = rt_device_find(SAMPLE_UART_NAME);
    /*打开 uart2*/
    ret = rt_device_open(serial, RT_DEVICE_FLAG_INT_RX|RT_DEVICE_FLAG_RDWR);//中断接收 读写打开
    if(ret<0)
    {
       rt_kprintf("open uart2 file.....\n");
    }
     rt_kprintf("open uart2 success...\n");
     /*创建接收信号量*/
     rx_sem = rt_sem_create("uart_rx_sem",0,RT_IPC_FLAG_FIFO);
     move_sem = rt_sem_create("move_command_sem",0,RT_IPC_FLAG_FIFO);
     dir_sem = rt_sem_create("dir_command_sem",0,RT_IPC_FLAG_FIFO);
     /*设置回调函数*/
     rt_device_set_rx_indicate(serial, uart_input);

     int i =0;
     while(1)
     {   /*等待串口接收到数据的信号量*/
         result = rt_sem_take(rx_sem, RT_WAITING_FOREVER);
         if (result != RT_EOK) {
         rt_kprintf("take a dynamic re_sem semaphore, failed.\n");
         rt_sem_delete(rx_sem);
         return; }
         else//接收到串口收到消息的信号量   接收命令 判断命令 发送缓冲区
         {
             char * tmp;
             /*接收4个字节的信息*/
             rt_device_read(serial,0,buffer,4);
             /*判断命令类型*/
             if(buffer[0]=='0')//判断命令类型
                 {tmp =command_move_pool;}
             else if (buffer[0]=='1')
                 {
                     tmp =command_dir_pool;
                 }
             else {//错误的命令类型
                 tmp=NULL;
            }

             if(tmp!=NULL){//命令格式正确 tmp 不是null
                      /*将四个字节转移到命令缓冲区*/
                 for(i=0;i<4;i++){
                 tmp[i]=buffer[i];
                 //rt_kprintf("%d",i);
                 }
             }
             else{
                 rt_kprintf("错误的命令类型.....\n");
             }


             /*接收完一个命令复位i*/
             if(i==4){
                 int j=0;
                 //打印命令
                 rt_kprintf("命令为：\n");
                                  for(j=0;j<4;j++)
                                  {
                                      rt_kprintf("%c",tmp[j]);
                                  };
                                  rt_kprintf("\n");

                 /*根据命令类型发送不同的信号量给控制进程*/
                 if(tmp==command_move_pool){
                     rt_sem_release(move_sem);
                 }
                 else if (tmp==command_dir_pool)
                 {
                     rt_sem_release(dir_sem);
                 }
                i=0;//复位i
             }
         }
    }
};



/*命令缓冲区
 * [0] [0/1/-1] [speed][保留]
 * [0]:代表是速度控制命令
 * [0/1/-1] 0 停止  1 前进 2 后退
 * 完整命令实例：  0 1 0 0
 * */
/*线程2 入口  解析move命令缓冲区命令执行命令*/
static void total_con_move_entry(void *parameter){
    rt_err_t ret;
    rt_device_t  pwm1_dev,pwm2_dev;
    /*初识化引脚*/
    set_motrol_pin();
      /*打开pwm1 pwm2*/
      pwm1_dev = rt_device_find("pwm1");
      pwm2_dev = rt_device_find("pwm2");
      /*使能pwm1 pwm2*/
      rt_pwm_enable((struct rt_device_pwm *)pwm1_dev, 1);
      rt_pwm_enable((struct rt_device_pwm *)pwm2_dev, 1);
    while(1)
    {
         /*等待信号量*/
         ret = rt_sem_take(move_sem, RT_WAITING_FOREVER);
                if (ret != RT_EOK) {
                rt_kprintf("take a dynamic move semaphore, failed.\n");
                rt_sem_delete(move_sem);
                return; }
                else//解析命令执行动作
                {
                    if(command_move_pool[1]=='1'){
                        //前进命令
                        rt_kprintf("set forhead\n");
                        //解析速度
                         int tmp1=0,tmp2=0,speed_tmp3=0;
                         tmp1=command_move_pool[2]-'0';
                         tmp2=command_move_pool[3]-'0';
                         speed_tmp3=tmp1*10+tmp2;
                        motrol_1_con(MOTROL_FORHEAD, speed_tmp3,pwm1_dev);
                        motrol_2_con(MOTROL_FORHEAD, speed_tmp3,pwm2_dev);
                    };
                    if(command_move_pool[1]=='0'){
                        rt_kprintf("set stop\n");
                        motrol_1_con(MOTROL_STOP, 0,pwm1_dev);
                        motrol_2_con(MOTROL_STOP, 0,pwm2_dev);
                    };
                    if(command_move_pool[1]=='2'){
                        rt_kprintf("set back\n");
                        int tmp1=0,tmp2=0,speed_tmp3=0;
                        tmp1=command_move_pool[2]-'0';
                        tmp2=command_move_pool[3]-'0';
                        speed_tmp3=tmp1*10+tmp2;
                         motrol_1_con(MOTROL_BACKWORD, speed_tmp3,pwm1_dev);
                         motrol_2_con(MOTROL_BACKWORD, speed_tmp3,pwm2_dev);
                    };
                }
    }
}
/*
 *[1] [pwm1] [pwm1] [speed]
 * [1]:代表是方向控制命令
 * [pwm1] 占空比的第一个数
 * [pwm2] 占空比的第二个数
 * [speed] 代表速度 1 代表10%的速度 2 20% 0 100%
 *完整命令实例：1 50 1
    线程3 入口  解析dir命令缓冲区命令执行命令*/
static void total_con_dir_entry(void *parameter){
    rt_err_t ret;
    int per,per_per=50;


    rt_device_t  pwm_dev;
    /*打开pwm3*/
    pwm_dev = rt_device_find("pwm3");
    /*使能pwm3*/
    rt_pwm_enable((struct rt_device_pwm *)pwm_dev, 1);
    /*初始化方向*/
    dir_init(pwm_dev);
       while(1)
       {
            /*等待信号量*/
            ret = rt_sem_take(dir_sem, RT_WAITING_FOREVER);
                   if (ret != RT_EOK) {
                   rt_kprintf("take a dynamic dir semaphore, failed.\n");
                   rt_sem_delete(dir_sem);
                   return; }
                   else//解析命令执行动作
                   {
                     /*获取角度*/
                       int shi,ge;
                       shi=command_dir_pool[1]-'0';
                       ge =command_dir_pool[2]-'0';

                     per=(shi*10+ge);
                     /*设置角度
                      * per 30~50~70*/
                     if(per==per_per){

                     }
                     else{
                         ch_dir(per, 100, pwm_dev);
                         per_per=per;
                     }


                     rt_kprintf(" thead get shi: %d\n",shi);
                     rt_kprintf("thead get ge: %d\n",ge);
                     rt_kprintf("per: %d\n",per);
                     rt_thread_mdelay(3);
                   }
       }
}


/*
 * 线程4 入口 encoder1
 *
 *
 * */
#define ENCODER_PIN  85
void encoder_irq(void *args){
    encoder_num++;
    if(encoder_num==54)
        encoder_num=0;
    LOG_I("LOG_I encoder_num %d:",encoder_num);
}
static void encoder1( void *parameter){
//引脚初识化

    rt_pin_mode(ENCODER_PIN, PIN_MODE_INPUT);
    rt_pin_attach_irq(ENCODER_PIN, PIN_IRQ_MODE_FALLING, encoder_irq, RT_NULL);
    /* 使 能 中 断 */
    rt_pin_irq_enable(ENCODER_PIN, PIN_IRQ_ENABLE);
    while(1)
    {
        rt_thread_mdelay(10);
    }
}

static int thead1(void){
    rt_thread_t tid1;
    /* 创 建 线 程 1， 名 称 是 uart2_re， 入 口 是 uart2_re*/
    tid1 = rt_thread_create("uart2_re",
    uart_re_entry, RT_NULL,
    THREAD_STACK_SIZE,
    THREAD_PRIORITY, THREAD_TIMESLICE);
    /* 如 果 获 得 线 程 控 制 块， 启 动 这 个 线 程 */
    if (tid1 != RT_NULL)
    rt_thread_startup(tid1);
    rt_kprintf("thead1 ok\n");
    return 0;
}
static int thead2(void){
    rt_thread_t tid2;
    /* 创 建 线 程 2， 名 称 是 total_con， 入 口 是 total_con_entry*/
    tid2 = rt_thread_create("con_move",
    total_con_move_entry, RT_NULL,
    THREAD_STACK_SIZE,
    25, THREAD_TIMESLICE);
    /* 如 果 获 得 线 程 控 制 块， 启 动 这 个 线 程 */
    if (tid2 != RT_NULL)
    rt_thread_startup(tid2);\
    rt_kprintf("thead2 ok\n");
    return 0;
}
static int thead3(void){
    rt_thread_t tid3;
    /* 创 建 线 程 3， 名 称 是 con_dir， 入 口 是 con_dir*/
    tid3 = rt_thread_create("con_dir",
    total_con_dir_entry, RT_NULL,
    THREAD_STACK_SIZE,
    26, THREAD_TIMESLICE);
    /* 如 果 获 得 线 程 控 制 块， 启 动 这 个 线 程 */
    if (tid3 != RT_NULL)
    rt_thread_startup(tid3);\
    rt_kprintf("thead3 ok\n");
    return 0;
}


static int thead4(void){
    rt_thread_t tid4;
    /* 创 建 线 程 4， 名 称 是 tid4， 入 口 是 encoder1*/
    tid4 = rt_thread_create("encoder1",
     encoder1, RT_NULL,
    THREAD_STACK_SIZE,
    26, THREAD_TIMESLICE);
    /* 如 果 获 得 线 程 控 制 块， 启 动 这 个 线 程 */
    if (tid4 != RT_NULL)
    rt_thread_startup(tid4);\
    rt_kprintf("thead4 ok\n");
    return 0;
}


int thead_creat(void){
    thead1();//启动线程1 uart 命令接收与转移到缓冲区
     // nrf命令接收 转移到缓冲区
    thead2();//启动线程2 move 缓冲区 命令解析与执行
    thead3();//启动线程3 dir  缓冲区命令解析与执行
    thead4();//启动线程4encoder1

    return 0;
};
