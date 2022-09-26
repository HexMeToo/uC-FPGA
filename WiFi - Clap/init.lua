outpin=3
pinValue=gpio.HIGH
gpio.mode(outpin,gpio.OUTPUT,pinValue)
gpio.write(outpin,pinValue)

SSID = "ssid"
PSWD = "password"
MASK = "255.255.255.0"

IPADR = "xxx.xxx.xxx.xxx"
FLAG = "0"

wifi.setmode(wifi.STATION)
wifi.sta.config(SSID,PSWD)
wifi.sta.autoconnect(1)

tmr.alarm(1,1000, 1, function()
   if wifi.sta.getip()==nil then
      print("Czekam na IP --> "..wifi.sta.status())
   else
      print("Nowy adres IP to --> "..wifi.sta.getip())
      IP_ESP, MASK, IP_ROUTER= wifi.sta.getip()
      print(IP_ESP, MASK2, IP_ROUTER)
      wifi.sta.disconnect()
      wifi.sta.setip({ip=IPADR, netmask=MASK, gateway=IP_ROUTER})
      tmr.stop(1)
      FLAG = "1"
      wifi.sta.connect()
   end
end)



tmr.alarm(2,1000, 1, function()
if FLAG == "1" then
   if wifi.sta.getip()==nil then
      print("Czekam na IP --> "..wifi.sta.status())
   else
      print("Nowy adres IP2 to --> "..wifi.sta.getip())
      tmr.stop(2)
   end
end
end)



responseHeader = function(code, type) 
    return "HTTP/1.1 " .. code .. "\r\nConnection: close\r\nServer: nunu-Luaweb\r\nContent-Type: " .. type .. "\r\n\r\n";  
end 

srv=net.createServer(net.TCP)
srv:listen(80,function(conn)
     conn:on("receive",function(conn,payload)
     conn:send(responseHeader("200 OK","text/html")); 
          function ctrlpower()
               value=string.sub(payload,param[2]+1,param[2]+1)
               if value=="1"  then
                    pinValue=gpio.LOW
                    gpio.write(outpin,pinValue)
                    tmr.delay(1000)
                    pinValue=gpio.HIGH                    
               end
               if value=="0" then
                    pinValue=gpio.LOW
               end
               gpio.write(outpin,pinValue)
               return pinValue
          end
          param={string.find(payload,"status=")}
          if param[2]~=nil then 
               pinValue=ctrlpower()
          end
          conn:send(pinValue)
          conn:on("sent",function(conn) conn:close() end)
     end)
end)
