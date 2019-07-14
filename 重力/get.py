import serial #导入模块


def Init(str) :
    portx=str
    bps=9600
    #超时设置,None：永远等待操作，0为立即返回请求结果，其他值为等待超时时间(单位为秒）
    timex=None
    ser=serial.Serial(portx,bps,timeout=timex)
    print("串口详情参数：", ser)
    return ser

#十六进制的读取

def get_angle(ser):
    ser.write(chr(0xff).encode("utf-8"))
    return [ser.read().hex(),ser.read().hex(),ser.read().hex(),ser.read().hex()]#读一个字节

def main() :
    ser = Init("COM6")
    angles = get_location(ser)
    print(angles)
    
    ser.close()#关闭串口


if __name__ == '__main__':
    main()
