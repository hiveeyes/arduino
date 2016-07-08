    #include "ADS1231.h" //include the declaration for this class
     
	static ADS1231_t ADS1231s[MAX_ADS1231];                       // static array of ADS1231 structures
	uint8_t ADS1231Count = 0;                                     // the total number of attached ADS1231
	
    //<<constructor>> setup the ADS1231
    ADS1231::ADS1231(){
	  if( ADS1231Count < MAX_ADS1231) {
		this->ADS1231Index = ADS1231Count++;  // assign a ADS1231 index to this instance
	  }
	  else
		this->ADS1231Index = INVALID_ADS1231 ;  // too many servos 
	}

    //<<destructor>>
    ADS1231::~ADS1231(){/*nothing to destruct*/}
     
	//turn on resp. off the ads
	void ADS1231::attach( uint8_t sclPin , uint8_t dataPin , uint8_t pwdnPin ) {
	  if( ADS1231Count < MAX_ADS1231) {
        volatile uint8_t *out;
        // store Pin numbers
		ADS1231s[this->ADS1231Index].sclBit =   digitalPinToBitMask(sclPin);  
		ADS1231s[this->ADS1231Index].sclPort =  digitalPinToPort(   sclPin);  
		ADS1231s[this->ADS1231Index].dataBit =  digitalPinToBitMask(dataPin);  
		ADS1231s[this->ADS1231Index].dataPort = digitalPinToPort(   dataPin);  
		ADS1231s[this->ADS1231Index].pwdnBit =  digitalPinToBitMask(pwdnPin);
		ADS1231s[this->ADS1231Index].pwdnPort = digitalPinToPort(   pwdnPin);
        // block interrupts
        uint8_t oldSREG = SREG;
        cli();
		// initialize the clock pin as an output:
        pinMode( 	  sclPin , OUTPUT );
        out = portOutputRegister(ADS1231s[this->ADS1231Index].sclPort);
		*out &= ~ADS1231s[this->ADS1231Index].sclBit;   // set sclPin low
		// initialize the data pin as an input:
        pinMode( 	  dataPin , INPUT  );
        out = portOutputRegister(ADS1231s[this->ADS1231Index].dataPort);
		*out &= ~ADS1231s[this->ADS1231Index].dataBit;   // set dataPin low
		// initialize the power down pin as an output:
		pinMode(pwdnPin, OUTPUT);
        // reenable interrupts
        SREG = oldSREG;        
		// reset ads1232 with a 10ms pulse on power down pin: 
        power(LOW);
		delay(10);
        power(HIGH);
	  }
	}

	 //turn on resp. off the ads
    void ADS1231::power( uint8_t power_mode ){
        volatile uint8_t *out;
        if( power_mode == LOW ) {
            out = portOutputRegister(ADS1231s[this->ADS1231Index].pwdnPort);
            *out &= ~ADS1231s[this->ADS1231Index].pwdnBit;   // set pwdnPin low        
        }
        else {
            out = portOutputRegister(ADS1231s[this->ADS1231Index].pwdnPort);
            *out |= ADS1231s[this->ADS1231Index].pwdnBit;    // set pwdnPin high        
        }
    }

    //check for new data ready
    uint8_t ADS1231::check(){
        if( *portInputRegister(ADS1231s[this->ADS1231Index].dataPort) & (ADS1231s[this->ADS1231Index].dataBit) ) {
            return LOW; }
        else {
            return HIGH; }
    }
     
    //read data from ads1231
    long ADS1231::readData(){
		long dataValue = 0;
        uint8_t inputStatus;
		for( uint8_t count = 0 ; count < 25 ; count++ ){
			// send pulse to SCL and read the data
			sclPulse();
            if( *portInputRegister(ADS1231s[this->ADS1231Index].dataPort) & (ADS1231s[this->ADS1231Index].dataBit) ) {
                inputStatus = HIGH; }
            else {
                inputStatus = LOW; }
			dataValue |= (long)(inputStatus) << (31 - count);
		}
		return dataValue;
    }

	//read data from ads1231 and calibrate
    long ADS1231::calibrate(){
		long dataValue = 0;
		dataValue = readData();
		sclPulse();
		return dataValue;
    }

	//send pulse to ads1231
    void ADS1231::sclPulse(){
        volatile uint8_t *out;
        out = portOutputRegister(ADS1231s[this->ADS1231Index].sclPort);
        *out |= ADS1231s[this->ADS1231Index].sclBit;    // set sclPin high        
//		delayMicroseconds(1);
        out = portOutputRegister(ADS1231s[this->ADS1231Index].sclPort);
        *out &= ~ADS1231s[this->ADS1231Index].sclBit;   // set sclPin low        
	}
