library IEEE;
use IEEE.STD_LOGIC_1164.all;
use IEEE.std_logic_unsigned.all;

entity Rejestr_Sumator is
	port(	CLK: in std_logic;
			R: in std_logic;
			A: in std_logic_vector(3 downto 0);
			B: in std_logic_vector(4 downto 0);
			QA: buffer std_logic_vector(3 downto 0);
			QB: buffer std_logic_vector(4 downto 0)	);
end Rejestr_Sumator;

architecture Behavioral of Rejestr_Sumator is
begin
	process(clk, R)
	begin
	
		if R = '1' then
			QA <= "1111";
			QB <= "00000";
		else
			if clk'event and clk = '1' then
				QA <= A;
				QB <= B;
			end if;
		end if;
		
	end process;
end Behavioral;