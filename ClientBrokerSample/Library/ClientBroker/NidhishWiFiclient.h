void sleep(int time);
void wakeup();


void Wifi_init(String Networkname,String password){
    char a;
    int i=0;
    Serial1.begin(115200);
    SerialUSB.println("Serial1 begin");


    Serial1.println("AT");
    while(true){
        if(Serial1.available()){
            a=Serial1.read();
            SerialUSB.print(a);
        }
        if(a=='O'){
            i=1;
        }
        else if(a=='K'&&i==1){
            i=3;
            break;
        }


    }


    i=0;
    Serial1.println("AT+GMR");
    while(true){
        if(Serial1.available()){
            a=Serial1.read();
            SerialUSB.print(a);
        }
        if(a=='O'){
            i=1;
        }
        if(a=='K'&&i==1){
            i=3;
            break;
        }

    }

    i=0;  
    Serial1.println("AT+CWMODE=1");
    while(true){
        if(Serial1.available()){
            a=Serial1.read();
            SerialUSB.print(a);
        }

        if(a=='O'){
            i=1;
        }
        else if(a=='g'){
            i=1;
        }
        if(a=='K'&&i==1){
            i=3;
            break;
        }else if(a=='e'&&i==1){
            i=3;
            break;
        }

    }
    i=0;  
    Serial1.println("AT+CIPMODE=0");
    while(true){
        if(Serial1.available()){
            a=Serial1.read();
            SerialUSB.print(a);
        }

        if(a=='O'){
            i=1;
        }
        else if(a=='g'){
            i=1;
        }
        if(a=='K'&&i==1){
            i=3;
            break;
        }else if(a=='e'&&i==1){
            i=3;
            break;
        }

    }

    i=0;
    Serial1.println("AT+CIPMUX=0");
    while(true){
        if(Serial1.available()){
            a=Serial1.read();
            SerialUSB.print(a);
        }

        if(a=='O'){
            i=1;
        } else if(a=='e'){
            i=1;
        }
        if(a=='K'&&i==1){
            i=3;
            break;
        } else if(a=='d'){
            i=3;
            break;
        }


    }
    i=0;

    Serial1.println("AT+CWQAP");
    while(true){
        if(Serial1.available()){
            a=Serial1.read();
            SerialUSB.print(a);
        }
        if(a=='O'){
            i=1;
        }
        if(a=='K'&&i==1){
            i=3;
            break;
        }

    }


    bool again = false;
    i=0;



    while(again==false){
        Serial1.println("AT+CWJAP=\""+Networkname+"\",\""+password+"\"");
        while(true){
            if(Serial1.available()){
                a=Serial1.read();
                SerialUSB.print(a);
            }
            if(a=='O'){
                i=1;
            }
            if(a=='K'&&i==1){

                i=3;
                again=true;
                break;
            }
            if(a=='L'){
                again=false;
                break;
            }
        }
    }
    i=0;
    Serial1.println("AT+CIFSR");
    while(true){
        if(Serial1.available()){
            a=Serial1.read();
            SerialUSB.print(a);
        }
        if(a=='O'){
            i=1;
        }
        if(a=='K'&&i==1){
            i=3;
            break;
        }

    }
    i=0;

    SerialUSB.println("");
    SerialUSB.println("Connected!!!");

}

void connecttoserver(String servername){
    bool firse=false;


    while(firse==false){ 

        Serial1.println("AT+CIPSTART=\"TCP\",\"" + servername + "\",80");

        int i=0;
        char a;
        while(true){
            if(Serial1.available()){
                a=Serial1.read();
                SerialUSB.print(a);
            }
            if(a=='O'){
                i=1;
            }
            if(a=='K'&&i==1){
                i=3;
                firse=true;
                break;
            }
            if(a=='D'){
                firse=false;
                break;
            }

        }
        if(firse==true){
            break;
        }

    }
}

void disconnectfromserver(){
    int i=0;
    char a;
    bool closehoja = false;

    while(closehoja==false){

        Serial1.println("AT+CIPCLOSE");

        while(true){
            if(Serial1.available()){
                a=Serial1.read();
                SerialUSB.print(a);
            }
            if(a=='O'){
                i=1;
            }
            if(a=='K'&&i==1){
                i=3;
                closehoja=true;
                break;
            }if(a=='R'){
                closehoja=false;
                break;
            }

        }
        if(closehoja=true){
            break;
        }
    }
}

void sendandreceive(String data,String servername,String datatype,String path){
    int i=0;
    char a;
    bool bakwas=false;

    String t1;
    while(bakwas==false){
        t1 = "POST "+path+" HTTP/1.1\r\nHost:"+servername+"\r\nContent-Type:"+datatype+"\r\nContent-Length:"+String(data.length())+"\r\n";
        SerialUSB.println(data.length());
        SerialUSB.println(t1.length()+data.length()+4);
        Serial1.print("AT+CIPSEND=");
        Serial1.println(t1.length()+data.length()+4);

        while(true){
            if(Serial1.available()){
                a=Serial1.read();
                SerialUSB.print(a);
            }
            if(a=='>'){
                bakwas=true;
                break;
            }
            if(a=='R'){
                bakwas=false;
                break;
            }


        }
        if(bakwas==true){
            break;
        }
    }


    Serial1.println(t1);
    //Serial1.println(t2);
    Serial1.println(data);
    while(true){
        if(Serial1.available()){
            a=Serial1.read();
            SerialUSB.print(a);
        }
        if(a=='O'){
            i=1;
        }
        if(a=='K'&&i==1){
            i=3;
            // karnayaar=true;
            break;

        }
        if(a=='L'){
            //  karnayaar=false;
            sendandreceive(data,servername,datatype,path);
            break;
        }

    }


    i=0;
    int x=0;
    SerialUSB.println("");
    SerialUSB.println("Working!!!");
    while(true){
        if(Serial1.available()){
            a=Serial1.read();
            SerialUSB.print(a);
        }
        if(a=='v'){

            x=i;
        }
        if(a=='e'&&i==x+1){


            break;


        }
        if(a=='C'){
            x=i;

        }
        if(a=='T'&&i==x+1){
            //Wifi_init(Networkname,password);
            //connectandsend(data,path,servername,Networkname,password,datatype,port);
            sendandreceive(data,servername,datatype,path);
            break;


        }

        i++;
    }




    SerialUSB.println("done!!");
    i=0;
    int k=0;
    while(true){
        if(Serial1.available()){
            a=Serial1.read();
            i++;
        }
        if(i==4){
            break;

        }







    }
    i=0;
    String str;


    int timeout = millis() + 5000;
    while (Serial1.available() == 0) {
        if (timeout - millis() < 0) {
            SerialUSB.println("Client Timeout!");
            //Serial1.stop();
            i++;
            break;
        }
    }
    //while(Serial1.available()==0);
    if(i==0){
        while(Serial1.available()) {
            str = Serial1.readStringUntil('\r');

        }
    }
    else{
        //Wifi_init(Networkname,password);
        //    connectandsend(data,path,servername,Networkname,password,datatype,port);
        sendandreceive(data,servername,datatype,path);
    }
}

String connectandsend(String data,String path,String servername,String Networkname,String password,String datatype,int port){

    char a;

    int i=0;



    bool karnayaar=false;
    while(karnayaar==false){

        bool firse=false;


        while(firse==false){ 
            Serial1.println("AT+CIPSTART=\"TCP\",\"" + servername + "\","+ String(port));

            i=0;
            while(true){
                if(Serial1.available()){
                    a=Serial1.read();
                    SerialUSB.print(a);
                }
                if(a=='O'){
                    i=1;
                }
                if(a=='K'&&i==1){
                    i=3;
                    firse=true;
                    break;
                }
                if(a=='D'){
                    firse=false;
                    break;
                }

            }
            if(firse==true){
                break;
            }

        }
        i=0;

        bool bakwas=false;

        String t1;
        while(bakwas==false){
            t1 = "POST "+path+" HTTP/1.1\r\nHost:"+servername+"\r\nContent-Type:"+datatype+"\r\nContent-Length:"+String(data.length())+"\r\n";
            SerialUSB.println(data.length());
            SerialUSB.println(t1.length()+data.length()+4);
            Serial1.print("AT+CIPSEND=");
            Serial1.println(t1.length()+data.length()+4);

            while(true){
                if(Serial1.available()){
                    a=Serial1.read();
                    SerialUSB.print(a);
                }
                if(a=='>'){
                    bakwas=true;
                    break;
                }
                if(a=='R'){
                    bakwas=false;
                    break;
                }


            }
            if(bakwas==true){
                break;
            }
        }


        Serial1.println(t1);
        //Serial1.println(t2);
        Serial1.println(data);
        while(true){
            if(Serial1.available()){
                a=Serial1.read();
                SerialUSB.print(a);
            }
            if(a=='O'){
                i=1;
            }
            if(a=='K'&&i==1){
                i=3;
                karnayaar=true;
                break;

            }
            if(a=='L'){
                karnayaar=false;
                // connectandsend(data,path,servername,Networkname,password);
                break;
            }

        }
        if(karnayaar==true){
            break;
        }
    }
    i=0;
    int x=0;
    SerialUSB.println("");
    SerialUSB.println("Working!!!");
    while(true){
        if(Serial1.available()){
            a=Serial1.read();
            SerialUSB.print(a);
        }
        if(a=='v'){

            x=i;
        }
        if(a=='e'&&i==x+1){


            break;


        }
        if(a=='C'){
            x=i;

        }
        if(a=='T'&&i==x+1){
            Wifi_init(Networkname,password);
            connectandsend(data,path,servername,Networkname,password,datatype,port);

            break;


        }

        i++;
    }




    SerialUSB.println("done!!");
    i=0;
    int k=0;
    while(true){
        if(Serial1.available()){
            a=Serial1.read();
            i++;
        }
        if(i==4){
            break;

        }







    }
    i=0;
    String str;
    //     while(true){
    //       if(Serial1.available()){
    //    a=Serial1.read();
    //   // if(a!='\n'){
    //    str+=a;
    //   // }
    //    
    //    }
    //    if(a=='}'){
    //      break;
    //    }
    // 
    //  
    //  
    //    i++;
    //    
    //     
    //    }

    int timeout = millis() + 5000;
    while (Serial1.available() == 0) {
        if (timeout - millis() < 0) {
            SerialUSB.println("Client Timeout!");
            //Serial1.stop();
            i++;
            break;
        }
    }
    //while(Serial1.available()==0);
    if(i==0){
        while(Serial1.available()) {
            str = Serial1.readStringUntil('\r');

        }
    }
    else{
        Wifi_init(Networkname,password);
        connectandsend(data,path,servername,Networkname,password,datatype,port);
    }




    //    delay(150);
    //  
    //  
    i=0;
    bool closehoja = false;

    while(closehoja==false){

        Serial1.println("AT+CIPCLOSE");

        while(true){
            if(Serial1.available()){
                a=Serial1.read();
                SerialUSB.print(a);
            }
            if(a=='O'){
                i=1;
            }
            if(a=='K'&&i==1){
                i=3;
                closehoja=true;
                break;
            }if(a=='R'){
                closehoja=false;
                break;
            }

        }
        if(closehoja=true){
            break;
        }
    }
    SerialUSB.println("");
    //    delay(150);
    return str;
}




String wakesendsleep(String Networkname,String password,String data,String path,String servername,String datatype,int port){
    Serial1.begin(115200);

    wakeup();
    //Wifi_init(Networkname,password);
    String str = connectandsend(data,path,servername,Networkname,password,datatype,port);

    sleep(10000);
    SerialUSB.println("");
    return str;
}

void sleep(int time){
    int i;
    char a;
    i=0;
    Serial1.println("AT+GSLP="+String(time));

    while(true){
        if(Serial1.available()){
            a=Serial1.read();
            SerialUSB.print(a);
        }
        if(a=='O'){
            i=1;
        }
        if(a=='K'&&i==1){
            i=3;
            break;
        }

    }

}

void wakeup(){
    char a;
    int i=0;

    pinMode(36,OUTPUT);
    digitalWrite(36,HIGH);
    for(int l=0;l<1000000;l++){
        asm("nop\r\n");}
    digitalWrite(36,LOW);
    for(int l=0;l<1000000;l++){
        asm("nop\r\n");}
    digitalWrite(36,HIGH);



    while(true){
        if(Serial1.available()){
            a=Serial1.read();
            // SerialUSB.print(a);
        }
        if(a=='i'){
            i=1;
        }
        if(a=='d'&&i==1){
            i=3;
            break;
        }

    }

}
