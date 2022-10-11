library IEEE;
use IEEE.STD_LOGIC_1164.all;
use IEEE.std_logic_unsigned.all;

entity Preskaler_1 is
	port(	clk: in std_logic;
			Q: buffer std_logic);
end Preskaler_1;

architecture Behavioral of Preskaler_1 is
begin
	process(clk)
	variable Licznik : integer range 0 to 24_999_999;
	begin
		if (clk'event and clk = '0') then
			if Licznik > 0 then
				Licznik := Licznik - 1;
			else
				Licznik := 24_999_999;
				Q <= Q xor '1';
			end if;
		end if;
	end process;
end Behavioral;