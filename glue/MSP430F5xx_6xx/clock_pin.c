/*
  pin setup for crystal-based clocks. 
  this file is generated automatically based on the device datasheets
  Author:          Petre Rodan <2b4eda@subdimension.ro>
  Available from:  https://github.com/rodan/reference_libs_msp430

  generated on Sat Nov 20 04:52:53 UTC 2021
*/

#include <msp430.h>

void clock_pin_init(void)
{
#ifdef USE_XT1

#if defined (__MSP430F5131__) || defined (__MSP430F5132__) \
 || defined (__MSP430F5151__) || defined (__MSP430F5152__) \
 || defined (__MSP430F5171__) || defined (__MSP430F5172__)
    PJSEL |= BIT5;
#elif defined (__MSP430F5212__) || defined (__MSP430F5214__) \
 || defined (__MSP430F5217__) || defined (__MSP430F5219__) \
 || defined (__MSP430F5222__) || defined (__MSP430F5224__) \
 || defined (__MSP430F5229__) || defined (__MSP430F5232__) \
 || defined (__MSP430F5234__) || defined (__MSP430F5237__) \
 || defined (__MSP430F5239__) || defined (__MSP430F5242__) \
 || defined (__MSP430F5244__) || defined (__MSP430F5247__) \
 || defined (__MSP430F5249__) || defined (__MSP430F5252__) \
 || defined (__MSP430F5253__) || defined (__MSP430F5254__) \
 || defined (__MSP430F5255__) || defined (__MSP430F5256__) \
 || defined (__MSP430F5257__) || defined (__MSP430F5258__) \
 || defined (__MSP430F5259__) || defined (__MSP430F5304__) \
 || defined (__MSP430F5308__) || defined (__MSP430F5309__) \
 || defined (__MSP430F5310__) || defined (__MSP430F5324__) \
 || defined (__MSP430F5325__) || defined (__MSP430F5326__) \
 || defined (__MSP430F5327__) || defined (__MSP430F5328__) \
 || defined (__MSP430F5329__) || defined (__MSP430F5340__) \
 || defined (__MSP430F5341__) || defined (__MSP430F5342__) \
 || defined (__MSP430F5500__) || defined (__MSP430F5501__) \
 || defined (__MSP430F5502__) || defined (__MSP430F5503__) \
 || defined (__MSP430F5504__) || defined (__MSP430F5505__) \
 || defined (__MSP430F5506__) || defined (__MSP430F5507__) \
 || defined (__MSP430F5508__) || defined (__MSP430F5509__) \
 || defined (__MSP430F5510__) || defined (__MSP430F5513__) \
 || defined (__MSP430F5514__) || defined (__MSP430F5515__) \
 || defined (__MSP430F5517__) || defined (__MSP430F5519__) \
 || defined (__MSP430F5521__) || defined (__MSP430F5522__) \
 || defined (__MSP430F5524__) || defined (__MSP430F5525__) \
 || defined (__MSP430F5526__) || defined (__MSP430F5527__) \
 || defined (__MSP430F5528__) || defined (__MSP430F5529__)
    P5SEL |= BIT4;
#elif defined (__CC430F5123__) || defined (__CC430F5125__) \
 || defined (__CC430F5133__) || defined (__CC430F5135__) \
 || defined (__CC430F5137__) || defined (__CC430F5143__) \
 || defined (__CC430F5145__) || defined (__CC430F5147__) \
 || defined (__CC430F6125__) || defined (__CC430F6126__) \
 || defined (__CC430F6127__) || defined (__CC430F6135__) \
 || defined (__CC430F6137__) || defined (__CC430F6147__)
    P5SEL |= BIT0;
#elif defined (__MSP430F5418__) || defined (__MSP430F5418A__) \
 || defined (__MSP430F5419__) || defined (__MSP430F5419A__) \
 || defined (__MSP430F5435__) || defined (__MSP430F5435A__) \
 || defined (__MSP430F5436__) || defined (__MSP430F5436A__) \
 || defined (__MSP430F5437__) || defined (__MSP430F5437A__) \
 || defined (__MSP430F5438__) || defined (__MSP430F5438A__)
    P7SEL |= BIT0;
#else
    #error "USE_XT1 was defined but pins not known in 'glue/MSP430F5xx_6xx/clock_pin.c'"
#endif
#endif

#ifdef USE_XT2
#if defined (__MSP430F5212__) || defined (__MSP430F5214__) \
 || defined (__MSP430F5217__) || defined (__MSP430F5219__) \
 || defined (__MSP430F5222__) || defined (__MSP430F5224__) \
 || defined (__MSP430F5229__) || defined (__MSP430F5232__) \
 || defined (__MSP430F5234__) || defined (__MSP430F5237__) \
 || defined (__MSP430F5239__) || defined (__MSP430F5242__) \
 || defined (__MSP430F5244__) || defined (__MSP430F5247__) \
 || defined (__MSP430F5249__) || defined (__MSP430F5252__) \
 || defined (__MSP430F5253__) || defined (__MSP430F5254__) \
 || defined (__MSP430F5255__) || defined (__MSP430F5256__) \
 || defined (__MSP430F5257__) || defined (__MSP430F5258__) \
 || defined (__MSP430F5259__) || defined (__MSP430F5304__) \
 || defined (__MSP430F5308__) || defined (__MSP430F5309__) \
 || defined (__MSP430F5310__) || defined (__MSP430F5324__) \
 || defined (__MSP430F5325__) || defined (__MSP430F5326__) \
 || defined (__MSP430F5327__) || defined (__MSP430F5328__) \
 || defined (__MSP430F5329__) || defined (__MSP430F5340__) \
 || defined (__MSP430F5341__) || defined (__MSP430F5342__) \
 || defined (__MSP430F5418__) || defined (__MSP430F5418A__) \
 || defined (__MSP430F5419__) || defined (__MSP430F5419A__) \
 || defined (__MSP430F5435__) || defined (__MSP430F5435A__) \
 || defined (__MSP430F5436__) || defined (__MSP430F5436A__) \
 || defined (__MSP430F5437__) || defined (__MSP430F5437A__) \
 || defined (__MSP430F5438__) || defined (__MSP430F5438A__) \
 || defined (__MSP430F5500__) || defined (__MSP430F5501__) \
 || defined (__MSP430F5502__) || defined (__MSP430F5503__) \
 || defined (__MSP430F5504__) || defined (__MSP430F5505__) \
 || defined (__MSP430F5506__) || defined (__MSP430F5507__) \
 || defined (__MSP430F5508__) || defined (__MSP430F5509__) \
 || defined (__MSP430F5510__) || defined (__MSP430F5513__) \
 || defined (__MSP430F5514__) || defined (__MSP430F5515__) \
 || defined (__MSP430F5517__) || defined (__MSP430F5519__) \
 || defined (__MSP430F5521__) || defined (__MSP430F5522__) \
 || defined (__MSP430F5524__) || defined (__MSP430F5525__) \
 || defined (__MSP430F5526__) || defined (__MSP430F5527__) \
 || defined (__MSP430F5528__) || defined (__MSP430F5529__)
    P5SEL |= BIT2;
#elif defined (__MSP430F5333__) || defined (__MSP430F5335__) \
 || defined (__MSP430F5336__) || defined (__MSP430F5338__) \
 || defined (__MSP430F5358__) || defined (__MSP430F5359__) \
 || defined (__MSP430F5630__) || defined (__MSP430F5631__) \
 || defined (__MSP430F5632__) || defined (__MSP430F5633__) \
 || defined (__MSP430F5634__) || defined (__MSP430F5635__) \
 || defined (__MSP430F5636__) || defined (__MSP430F5637__) \
 || defined (__MSP430F5638__) || defined (__MSP430F5658__) \
 || defined (__MSP430F5659__) || defined (__MSP430F6433__) \
 || defined (__MSP430F6435__) || defined (__MSP430F6436__) \
 || defined (__MSP430F6438__) || defined (__MSP430F6458__) \
 || defined (__MSP430F6459__) || defined (__MSP430F6630__) \
 || defined (__MSP430F6631__) || defined (__MSP430F6632__) \
 || defined (__MSP430F6633__) || defined (__MSP430F6634__) \
 || defined (__MSP430F6635__) || defined (__MSP430F6636__) \
 || defined (__MSP430F6637__) || defined (__MSP430F6638__) \
 || defined (__MSP430F6658__) || defined (__MSP430F6659__) \
 || defined (__MSP430FG6425__) || defined (__MSP430FG6426__) \
 || defined (__MSP430FG6625__) || defined (__MSP430FG6626__)
    P7SEL |= BIT2;
#else
    #error "USE_XT2 was defined but pins not known in 'glue/MSP430F5xx_6xx/clock_pin.c'"
#endif
#endif
}
