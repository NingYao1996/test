
#include "gd32f3x0.h"
#include "beep.h"
#include "SystemManage.h"
#include "SystemBase.h"



///////////////////////不使用PWM的buzzer程序/////////////////////////////////////////////////
#define BUZZER_RCU    RCU_GPIOB
#define BUZZER_PORT   GPIOB
#define BUZZER_PIN    GPIO_PIN_9

#define BUZZERTimer      TIMER16
#define BUZZERTimer_RCC  RCU_TIMER16

#define BUZZERPWM_Frequency  4000 //Hz
#define BUZZERTimer_Period   (rcu_clock_freq_get(CK_SYS)/BUZZERPWM_Frequency)


extern u8 bat_init_status(void);


void BUZZERInit(void )
{  
    timer_oc_parameter_struct timer_ocintpara;
    timer_parameter_struct timer_initpara;
 
	rcu_periph_clock_enable(BUZZER_RCU);
	
    gpio_mode_set(BUZZER_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, BUZZER_PIN );
    gpio_output_options_set(BUZZER_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,BUZZER_PIN );
    gpio_af_set(BUZZER_PORT, GPIO_AF_2, BUZZER_PIN);
    
	rcu_periph_clock_enable(BUZZERTimer_RCC);
			
    /* TIMER1 configuration */
    timer_initpara.prescaler         = 0;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = BUZZERTimer_Period;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(BUZZERTimer,&timer_initpara);

     /* CH1 configuration in PWM mode */
    timer_ocintpara.outputstate  = TIMER_CCX_ENABLE;
    timer_ocintpara.outputnstate = TIMER_CCXN_DISABLE;
    timer_ocintpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
    timer_ocintpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
    timer_ocintpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
    timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;
    timer_channel_output_config(BUZZERTimer,TIMER_CH_0,&timer_ocintpara);

    timer_channel_output_pulse_value_config(BUZZERTimer,TIMER_CH_0,0);
    timer_channel_output_mode_config(BUZZERTimer,TIMER_CH_0,TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(BUZZERTimer,TIMER_CH_0,TIMER_OC_SHADOW_DISABLE);

    timer_primary_output_config(BUZZERTimer,ENABLE);
    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(BUZZERTimer);
    timer_enable(BUZZERTimer);  
  
}

vu16 BeepTime=0;
u16  BeepDelay=0;

void BuzzerInit(void)
{
	BUZZERInit();
	BeepTime=0;	
}



void BuzzerStop(void)
{	
	if(BeepDelay<200)
	{
	    BeepDelay++;
	}
	
	if(BeepTime>0)
	{
	    BeepTime--;
		if(BeepTime==0)
		{
		    timer_channel_output_pulse_value_config(BUZZERTimer,TIMER_CH_0,0);
		}
	}
}

void BuzzerStart(u16 time)
{
	if(BeepDelay<200)
	{
	    return ;
	}
	
    BeepTime=time;
	timer_channel_output_pulse_value_config(BUZZERTimer,TIMER_CH_0,(BUZZERTimer_Period*50)/100);
}


void PowerUpSound(void)
{
    static u8 flage=0;
	static u8 cnt=0;
	if(HAVENOSN==s_base.SN && flage==0  && BeepDelay==200)
	{	    
		cnt++;
		if(cnt==1 || cnt == 28 || cnt == 56)
		{
		     BuzzerStart(20);
		}
	    
		if(cnt>56)
		{
		   flage=1;
		}
	}
}
