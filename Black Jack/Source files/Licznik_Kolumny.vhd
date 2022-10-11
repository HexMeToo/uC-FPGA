library IEEE;
use IEEE.STD_LOGIC_1164.all;
use IEEE.std_logic_unsigned.all;

entity Licznik_Kolumny is
	port(	clk: in std_logic;
			Q: buffer std_logic_vector(3 downto 0) );
end Licznik_Kolumny;

architecture Behavioral of Licznik_Kolumny is
begin
	process(clk)
	begin
		if clk'event and clk = '1' then
			Q <= Q + 1;
		end if;
	end process;
end Behavioral;