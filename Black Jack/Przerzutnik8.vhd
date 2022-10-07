library IEEE;
use IEEE.STD_LOGIC_1164.all;
use IEEE.std_logic_unsigned.all;

entity Przerzutnik8 is
	port(	clk: in std_logic;
			D:	in std_logic_vector(7 downto 0);
			Q: out std_logic_vector(7 downto 0));
end Przerzutnik8;

architecture Behavioral of Przerzutnik8 is
begin
	process(clk)
	begin
		if clk'event and clk = '1' then
			Q(0) <= D(0);
			Q(1) <= D(1);
			Q(2) <= D(2);
			Q(3) <= D(3);
			Q(4) <= D(4);
			Q(5) <= D(5);
			Q(6) <= D(6);
			Q(7) <= D(7);
		end if;
	end process;
	
end Behavioral;