clear
clc

## Amplitude
W = 10000;

## TableLength
P = 101;

## Name
name = "inverseSinePower";

a = [0:W/(P-1):W];
inverse_power_sinus = round(-(W/pi)*(asin(1-2*a/W)-pi/2));

plot(a,inverse_power_sinus,'bo-');

title(strcat(name, " Created Table"));
set(gca,'FontSize',24);

# Print for C Code
printf("/* The table is created With Octave PC Application */");
printf("\nconst uint16_t %sAmplitude = %d;" , name, W);
printf("\nconst uint16_t %sLength = %d;" , name, P);
printf("\nconst uint16_t %sTable[%d] = {", name, P);
for ( i = 1:10:(length(a)))
  printf("\n");
  for ( j = 0:9)
    printf("%4d",inverse_power_sinus(i+j));
    if((i+j)>=length(a))break;
    endif
  printf(", ");
  endfor
endfor
printf("}; \n");
