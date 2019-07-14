import serial #导入模块
segtable = [0,1,2,3,4,5,6,7,8,9,'a','b','c','d','e','f','-']
portx="COM6"
bps=9600
#超时设置,None：永远等待操作，0为立即返回请求结果，其他值为等待超时时间(单位为秒）
timex=None
ser=serial.Serial(portx,bps,timeout=timex)
print("串口详情参数：", ser)


#十六进制的读取

while True : 
    ser.write(chr(0xff).encode("utf-8"))
    print(ser.read().hex(),ser.read().hex(),ser.read().hex(),ser.read().hex())#读一个字节
print("---------------")
ser.close()#关闭串口
