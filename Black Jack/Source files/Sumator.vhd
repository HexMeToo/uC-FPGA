library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity Sumator is
   port(
      A: in unsigned(3 downto 0);
		B: in unsigned(4 downto 0);
      sum: out unsigned(4 downto 0)); 
end entity Sumator;
 
architecture Behavioral of Sumator is
signal temp : unsigned(4 downto 0); 
begin 
   temp <= ("0" & A) + B; 
   sum       <= temp(4 downto 0); 
end architecture Behavioral;