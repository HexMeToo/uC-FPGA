library IEEE;
use IEEE.STD_LOGIC_1164.all;
use IEEE.std_logic_unsigned.all;

entity Generator is
	port(	CLK: in std_logic;
			Q: buffer std_logic_vector(3 downto 0) );
end Generator;

architecture Behavioral of Generator is
begin
	process(clk)
	begin
		if clk'event and clk = '1' then
			if Q < "1101" then
				Q <= Q + 1;
			else
				Q <= "0000";
			end if;
		end if;
	end process;
end Behavioral;