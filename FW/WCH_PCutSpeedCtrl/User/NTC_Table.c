/*
 * NTC_Table.c
 *
 *  Created on: 3 Dec 2024
 *      Author: hass
 */
/**
* The NTC table has 65 interpolation points.
* Unit:0.1 ¡ãC
*
*/
int NTC_table[65] = {
  3437, 2864, 2291, 2003, 1815, 1677, 1568,
  1479, 1403, 1337, 1279, 1227, 1180, 1137,
  1097, 1060, 1025, 992, 961, 931, 903, 876,
  850, 825, 801, 777, 754, 732, 710, 689, 668,
  647, 627, 607, 587, 567, 547, 528, 508, 489,
  469, 449, 429, 409, 389, 369, 348, 327, 305,
  282, 259, 235, 210, 184, 157, 127, 96, 61,
  24, -19, -69, -130, -209, -332, -455
};



/**
* \brief    Converts the ADC result into a temperature value.
*
*           P1 and p2 are the interpolating point just before and after the
*           ADC value. The function interpolates between these two points
*           The resulting code is very small and fast.
*           Only one integer multiplication is used.
*           The compiler can replace the division by a shift operation.
*
*           In the temperature range from -10¡ãC to 120¡ãC the error
*           caused by the usage of a table is 0.143¡ãC
*
* \param    adc_value  The converted ADC result
* \return              The temperature in 0.1 ¡ãC
*
*/
int NTC_ADC2Temperature(unsigned int adc_value){

  int p1,p2;
  /* Estimate the interpolating point before and after the ADC value. */
  p1 = NTC_table[ (adc_value >> 4)  ];
  p2 = NTC_table[ (adc_value >> 4)+1];

  /* Interpolate between both points. */
  return p1 - ( (p1-p2) * (adc_value & 0x000F) ) / 16;
};
