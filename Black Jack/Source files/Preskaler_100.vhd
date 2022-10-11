library IEEE;
use IEEE.STD_LOGIC_1164.all;
use IEEE.std_logic_unsigned.all;

entity Preskaler_100 is
	port(	clk: in std_logic;
			Q: buffer std_logic);
end Preskaler_100;

architecture Behavioral of Preskaler_100 is
begin
	process(clk)
	variable Licznik : integer range 0 to 15;
	begin
		if (clk'event and clk = '0') then
			if Licznik > 0 then
				Licznik := Licznik - 1;
			else
				Licznik := 15;
				Q <= Q xor '1';
			end if;
		end if;
	end process;
end Behavioral;